utils::globalVariables(c())

#' Create dummy inputs for test simulations
#'
#' \code{ecoregionMap}is a raster of all the unique groupings.
#'
#' @template rasterToMatch
#'
#' @return a \code{RasterLayer} object or, in the case of \code{makeDummyEcoregionFiles}, a list.
#'
#' @export
#' @importFrom raster mask res
#' @importFrom SpaDES.tools randomPolygons
#' @rdname dummy-inputs
makeDummyEcoregionMap <- function(rasterToMatch) {
  ecoregionMap <- randomPolygons(ras = rasterToMatch,
                                 res = res(rasterToMatch),
                                 numTypes = 2)
  ecoregionMap <- mask(ecoregionMap, rasterToMatch)
  return(ecoregionMap)
}

#' @details
#' \code{rawBiomassMap} is a raster of "raw" total stand biomass per pixel,
#'      with values between 100 and 20000 g/m^2.
#'
#' @export
#' @importFrom raster mask setValues getValues
#' @importFrom SpaDES.tools gaussMap
#' @importFrom scales rescale
#' @rdname dummy-inputs
makeDummyRawBiomassMap <- function(rasterToMatch) {
  rawBiomassMap <- gaussMap(rasterToMatch)
  rawBiomassMap <- setValues(rawBiomassMap,
                             rescale(getValues(rawBiomassMap), c(100, 20000)))
  rawBiomassMap <- mask(rawBiomassMap, rasterToMatch)
  return(rawBiomassMap)
}

#' @details
#' \code{standAgeMap} is a raster of stand age per pixel (where biomass exists)
#'      with values between 1 and 300 years.
#'
#' @param rawBiomassMap a \code{rawBiomassMap} (e.g. the one used
#'     throughout the simulation)
#'
#' @export
#' @importFrom raster setValues
#' @importFrom scales rescale
#' @rdname dummy-inputs
makeDummyStandAgeMap <- function(rawBiomassMap) {
  standAgeMap <- setValues(rawBiomassMap, asInteger(rescale(getValues(rawBiomassMap), c(1, 300))))
  return(standAgeMap)
}

#' @details
#' \code{rstLCC} is a raster land-cover class per pixel, with values between 1 and 5 that have no
#'      correspondence to any real land-cover classes.
#'
#' @export
#' @importFrom raster mask res
#' @importFrom SpaDES.tools randomPolygons
#' @rdname dummy-inputs
makeDummyRstLCC <- function(rasterToMatch) {
  rstLCC <- randomPolygons(ras = rasterToMatch,
                           res = res(rasterToMatch),
                           numTypes = 5)
  rstLCC <- mask(rstLCC, rasterToMatch)
  return(rstLCC)
}

#' @details
#' \code{ecoregionFiles} uses dummy versions of \code{ecoregionMap} and \code{rstLCC}
#' to create a list with two objects: the \code{ecoregionMap} and a table summarizing its
#' information per \code{pixelID}.
#' See \code{ecoregionProducer} (it uses \code{ecoregionProducer} internally).
#'
#' @param ecoregionMap a raster of all the unique groupings.
#' @param rstLCC a raster land-cover class per pixel,
#'
#' @export
#' @importFrom data.table data.table
#' @importFrom raster setValues
#' @importFrom stats complete.cases
#' @rdname dummy-inputs
makeDummyEcoregionFiles <- function(ecoregionMap, rstLCC, rasterToMatch) {
  ecoregionstatus <- data.table(active = "yes",
                                ecoregion = unique(ecoregionMap[]))
  ecoregionstatus <- ecoregionstatus[complete.cases(ecoregionstatus)]

  ecoregionFiles <- Cache(ecoregionProducer,
                          ecoregionMaps = list(ecoregionMap, rstLCC),
                          ecoregionName = "ECODISTRIC",
                          rasterToMatch = rasterToMatch,
                          userTags = "ecoregionFiles")
  return(ecoregionFiles)
}
