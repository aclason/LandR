% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/serotiny-resprouting.R
\name{doSerotiny}
\alias{doSerotiny}
\title{Activate serotiny after a (fire) disturbance}
\usage{
doSerotiny(burnedPixelCohortData, postFirePixelCohortData,
  postFireRegenSummary = NULL, species, sufficientLight,
  speciesEcoregion, simuTime, treedFirePixelTableSinceLastDisp,
  calibrate = FALSE)
}
\arguments{
\item{burnedPixelCohortData}{A /code{cohortData} table expanded to pixelIndex containing the only the cohorts in
the pixels that were affected by the disturbance}

\item{postFirePixelCohortData}{an empty /code{cohortData}-like table with columns "age", "B", "mortality",
"aNPPAct", and "sumB" removed and "pixelIndex" added.}

\item{postFireRegenSummary}{a data.table summarizing for which species serotiny/resprouting were
activated and in how many pixels, for each year. Only necessary if /code{calibrate = TRUE}.}

\item{species}{a \code{data.table} with species traits such as longevity, shade tolerance, etc.}

\item{sufficientLight}{a data.table containing probability of establishment, given a site's light conditions (X0-X5) for
each level of a species shade tolerance (1-5)}

\item{speciesEcoregion}{a \code{data.table} with \code{speciesEcoregion} values}

\item{simuTime}{integer. The current simulation time obtained with \code{time(sim)}}

\item{treedFirePixelTableSinceLastDisp}{a vector of pixels that burnt and were forested in the previous time step.}

\item{calibrate}{logical. Determines whether to output /code{postFirePixelCohortData}. Defaults to FALSE}
}
\value{
A list of objects. /code{postFirePixelCohortData}, a data.table with the cohorts that will undergo serotiny;
   /code{serotinyPixel} a vector of pixels where serotiny was activated;
   /code{postFireRegenSummary} the updated postFireRegenSummary, if /code{calibrate = TRUE}
}
\description{
Activate serotiny after a (fire) disturbance
}