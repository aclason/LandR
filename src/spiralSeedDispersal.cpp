#include <numeric>
// #include <math.h> /* round, floor, ceil, trunc */
  #include <Rcpp.h>
  using namespace Rcpp;

// [[Rcpp::export]]
Rcpp::IntegerVector which2(Rcpp::LogicalVector x) {
  Rcpp::IntegerVector v = Rcpp::seq(0, x.size()-1);
  return v[x];
}


//' Multiplies two doubles
//'
//' @param cellCoords Matrix, 2 columns, of x-y coordinates of the Receive cells
//' @param speciesVectorsList A list, where each element is a vector of NA and the speciesCode
//'   value of the list. The length of each vector MUST be the number of cells in the
//'   raster whose \code{cellCoords} are provided.
//' @param rcvSpeciesByIndex A list of length \code{NROW(cellCoords)} where each element
//'   is the vector of speciesCodes that are capable of being received in the
//'   corresponding \code{cellCoords}
//' @param speciesTable A data.table with species traits. Must have column 3 be
//'   \code{seeddistance_max}, column 2 be \code{seeddistance_eff}, and sorted in
//'   increasing order on the first column, speciesCode. The speciesCode values must
//'   be \code{seq(1, NROW(speciesTable))}
//' @param numCols Integer, number of columns in the raster whose \code{cellCoords}
//'   were provided
//' @param numCells Integer, number of cells in the raster whose \code{cellCoords}
//'   were provided
//' @param cellSize Integer, the \code{res(ras)[1]} of the raster whose \code{cellCoords}
//'   were provided
//' @param xmin Integer, the \code{xmin(ras)} of the raster whose \code{cellCoords}
//'   were provided
//' @param ymin Integer, the \code{ymin(ras)} of the raster whose \code{cellCoords}
//'   were provided
//' @param k Numeric, parameter passed to Ward dispersal kernel
//' @param b Numeric, parameter passed to Ward dispersal kernel
//' @param successionTimestep Integer, Same as Biomass_core.
//' @param verbose Logical, length 1. If \code{TRUE}, there will be some messaging.
//'   \code{FALSE} is none. It appears to be noticeably faster when this is
//'   \code{FALSE} (the default)
//' @return A logical matrix with ncols = \code{length(speciesVectorsList)} and nrows =
//'   \code{NROW(cellCoords)}, indicating whether that cellCoords successfully
//'   received seeds from each species.
//' @export
// [[Rcpp::export]]
LogicalMatrix spiralSeedDispersal( IntegerMatrix cellCoords,
                       Rcpp::List speciesVectorsList, List rcvSpeciesByIndex,
                       NumericMatrix speciesTable,
                       int numCols, int numCells, int cellSize, int xmin, int ymin,
                       double k, double b, double successionTimestep, bool verbose = 0)
{
  // List spiralSeedDispersal( IntegerMatrix cellCoords, double overallMaxDist,

  int nCellsRcv(cellCoords.nrow());
  int nSpeciesEntries(speciesTable.nrow());
  int x, y, dx, dy, spiralIndex;
  int spiralIndexMax = 10000000; // not really used except for debugging, it can be shrunk
  bool underMaxDist = true;

  // max distances by species
  NumericVector maxDistsSpV = speciesTable(_, 2);
  double overallMaxDist = max(maxDistsSpV);
  double overallMaxDistCorner = overallMaxDist * sqrt(2);
  NumericVector effDistsSpV = speciesTable(_, 1);
  NumericVector effDistNotNA;
  NumericVector maxDistNotNA;
  NumericVector numActiveCellsByRcvSp(nSpeciesEntries);
  NumericVector numActiveCellsByRcvSpDone(nSpeciesEntries);
  double maxOfMaxDists, newOverallMaxDistCorner;

  // coordinates and distances
  NumericVector xCoord;
  NumericVector yCoord;
  NumericVector dis1;
  int pixelSrc, pixelVal;
  bool notNegative, notTooBig, inequ;

  // output
  LogicalMatrix seedsArrivedMat(nCellsRcv, nSpeciesEntries);

  // Spiral mechanism
  x = y = dx = 0;
  spiralIndex = -1;
  dy = -1;
  int t = overallMaxDist;

  // messaging
  int floorOverallMaxDist = floor(overallMaxDist / 10);
  int moduloVal;
  if (cellSize < floorOverallMaxDist) {
    moduloVal = floorOverallMaxDist;
  } else {
    moduloVal = cellSize;
  }
  int curModVal = moduloVal;
  int curMessage = 0;
  double possCurModVal;
  int possCurMessage, disInt;

  // Primary "spiral" loop, one pixel at a time. Only calculate for the "generic" pixel,
  // which is effectively an offset from the "central" pixel;
  // then add this offset to cellCoods matrix of initial cells.
  // This will create a square-ish shape, i.e., make a square then add a single
  // pixel width around entire square to make a new slightly bigger square.
  while(underMaxDist == true && spiralIndex < spiralIndexMax) {
    spiralIndex += 1;
    NumericVector numActiveCellsByRcvSp(nSpeciesEntries); // need to rezero
    numActiveCellsByRcvSp = numActiveCellsByRcvSp + numActiveCellsByRcvSpDone;

    xCoord = x * cellSize + cellCoords(_, 0);
    yCoord = y * cellSize + cellCoords(_, 1);
    dis1[0] = sqrt(pow(cellCoords(0, 0) - xCoord[0], 2.0) +
      pow(cellCoords(0, 1) - yCoord[0], 2.0) );

    ////////////////////////////////////
    // messaging for progress
    if (verbose) {
      disInt = floor(dis1[0]/ sqrt(2));
      possCurModVal = disInt % moduloVal;
      possCurMessage = floor(disInt / moduloVal) * moduloVal;
      if (possCurModVal < curModVal && possCurMessage > curMessage)  {
        curMessage = possCurMessage;
        Rcpp::Rcout << "Dispersal distance completed: " << curMessage << " of " << overallMaxDist << std::endl;
      }
      curModVal = possCurModVal;
    }
    // End messaging for progress
    ////////////////////////////////////////////////////

    if (dis1[0] <= overallMaxDist) { // make sure to omit the corners of the square due to circle

      // Loop around each of the original cells
      for (int cellRcvInd = 0; cellRcvInd < nCellsRcv; ++cellRcvInd) {
        IntegerVector speciesPixelRcvPool = rcvSpeciesByIndex[cellRcvInd];

        pixelSrc = (numCols - ((yCoord[cellRcvInd] - ymin + cellSize/2)/cellSize)) * numCols +
          (xCoord[cellRcvInd] - xmin + cellSize/2)/cellSize;

        notNegative = pixelSrc > 0;
        notTooBig = pixelSrc <= numCells;
        if (notNegative && notTooBig) {

          for(IntegerVector::iterator speciesPixelRcv = speciesPixelRcvPool.begin();
              speciesPixelRcv != speciesPixelRcvPool.end(); ++speciesPixelRcv) {

            IntegerVector speciesVector = speciesVectorsList[*speciesPixelRcv - 1];
            pixelVal = speciesVector[pixelSrc - 1];
            numActiveCellsByRcvSp[*speciesPixelRcv - 1] += 1;

            inequ = 0;
            if (pixelVal >= 0) { // covers NA which is -2147483648
              NumericVector effDist(1);
              effDist = effDistsSpV[*speciesPixelRcv - 1];
              NumericVector maxDist(1);
              maxDist = maxDistsSpV[*speciesPixelRcv - 1];
              NumericVector dis(1);
              dis = dis1[0];
              if (is_true(all(dis <= maxDist))) {
                // Rcpp::Rcout << "------- pixelVal: " << pixelVal << std::endl;
                // Rcpp::Rcout << "dis : " << dis << std::endl;
                // Rcpp::Rcout << "effDist : " << effDist << std::endl;
                // Rcpp::Rcout << "maxDist : " << maxDist << std::endl;
                // Rcpp::Rcout << "effDistSpV : " << effDistsSpV << std::endl;
                // Rcpp::Rcout << "maxDistSpV : " << maxDistsSpV << std::endl;
                if (dis[0] == 0) {
                  inequ = 1;
                } else {
                  NumericVector dispersalProb =
                    ifelse(cellSize <= effDist,
                           ifelse(dis <= effDist,
                                  exp((dis - cellSize) * log(1 - k)/effDist) - exp(dis * log(1 - k)/effDist),
                                  (1 - k) * exp((dis - cellSize - effDist) * log(b)/maxDist) - (1 - k) * exp((dis - effDist) * log(b)/maxDist)),
                                  ifelse(dis <= cellSize,
                                         exp((dis - cellSize) * log(1 - k)/effDist) - (1 - k) * exp((dis - effDist) * log(b)/maxDist),
                                         (1 - k) * exp((dis - cellSize - effDist) * log(b)/maxDist) - (1 - k) * exp((dis - effDist) * log(b)/maxDist)));
                  dispersalProb = 1 - pow(1 - dispersalProb, successionTimestep);
                  double ran = R::runif(0, 1);
                  inequ = ran < dispersalProb[0];
                }

                seedsArrivedMat(cellRcvInd, *speciesPixelRcv - 1) = inequ || seedsArrivedMat(cellRcvInd, *speciesPixelRcv - 1);
                if (inequ) {
                  IntegerVector speciesPixelRcvIV(1);
                  speciesPixelRcvIV = *speciesPixelRcv;// [[Rcpp::export]]
                  IntegerVector rcvSpeciesByIndexIV = rcvSpeciesByIndex[cellRcvInd];
                  rcvSpeciesByIndex[cellRcvInd] = setdiff(rcvSpeciesByIndexIV, speciesPixelRcvIV);
                  rcvSpeciesByIndexIV = rcvSpeciesByIndex[cellRcvInd];
                  numActiveCellsByRcvSp[*speciesPixelRcv - 1] = numActiveCellsByRcvSp[*speciesPixelRcv - 1] - 1;
                }
              }

            }
          }
        }
      }

      // Pre-empt further searching if all rcv cells have received a given species
      // If, say, all rcv cells that can get species 2 all have gotten species 2, then
      // can remove species 2 and lower the overallMaxDistCorner accordingly
      LogicalVector rcvSpDone = numActiveCellsByRcvSp == 0;
      if (is_true(any(rcvSpDone))) {
        numActiveCellsByRcvSpDone = -1 * rcvSpDone;
        maxDistsSpV[rcvSpDone] = 0;
        maxOfMaxDists = max(maxDistsSpV);
        newOverallMaxDistCorner = maxOfMaxDists * sqrt(2);
        if (newOverallMaxDistCorner < overallMaxDistCorner) {
          IntegerVector rcvSpCodesDone = which2(rcvSpDone) + 1;
          if (verbose) {
            Rcpp::Rcout << "Species " << rcvSpCodesDone << " complete. New max dispersal distance: " << maxOfMaxDists << std::endl;
          }
          overallMaxDistCorner = newOverallMaxDistCorner;
          overallMaxDist = maxOfMaxDists;
        }
      }
    } // end skip corners of distance

    if (dis1[0] > overallMaxDistCorner) {
      underMaxDist = false;
    }


    if( (x == y) || ((x < 0) && (x == -y)) || ((x > 0) && (x == 1 - y))){
      t = dx;
      dx = -dy;
      dy = t;
    }
    // xKeep.push_back(x);
    // yKeep.push_back(y);
    // spiralIndexKeep.push_back(spiralIndex);
    // disKeep.push_back(dis1[0]);

    x += dx;
    y += dy;

  }


  // return List::create(
  //   _["x"] = xKeep,
  //   _["y"] = yKeep,
  //   _["dis"] = disKeep,
  //   _["spiralIndexKeep"] = spiralIndexKeep,
  //   _["dispersalProbKeep"] = dispersalProbKeep,
  //   _["seedsArrived"] = seedsArrivedMat
  // );
  return seedsArrivedMat;
  // return dis1;
}
