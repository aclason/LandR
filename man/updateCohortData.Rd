% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/cohorts.R
\name{updateCohortData}
\alias{updateCohortData}
\alias{.initiateNewCohorts}
\title{Add cohorts to \code{cohortData} and \code{pixelGroupMap}}
\usage{
updateCohortData(
  newPixelCohortData,
  cohortData,
  pixelGroupMap,
  currentTime,
  speciesEcoregion,
  treedFirePixelTableSinceLastDisp = NULL,
  successionTimestep,
  cohortDefinitionCols = c("pixelGroup", "age", "speciesCode"),
  verbose = getOption("LandR.verbose", TRUE),
  doAssertion = getOption("LandR.assertions", TRUE)
)

.initiateNewCohorts(
  newPixelCohortData,
  cohortData,
  pixelGroupMap,
  currentTime,
  cohortDefinitionCols = c("pixelGroup", "speciesCode", "age"),
  speciesEcoregion,
  successionTimestep
)
}
\arguments{
\item{newPixelCohortData}{must be a complete \code{cohortData} object with newly created
cohorts. They do not have to have \code{pixelGroup} values yet;
they can be overlapping with \code{cohortData}, (i.e., they can be regenerated on empty
pixels or on already occupied pixels).
Must contain the columns: \code{pixelIndex}, \code{speciesCode}, \code{ecoregionGroup}.
The remaining 4 (see \code{cohortData}) will be created with \code{0}s.}

\item{cohortData}{A \code{data.table} with columns: \code{pixelGroup}, \code{ecoregionGroup},
\code{speciesCode}, and optionally \code{age}, \code{B}, \code{mortality}, \code{aNPPAct},
and \code{sumB}.}

\item{pixelGroupMap}{A \code{RasterLayer} with pixel values equal to a pixel group
number that corresponds exactly to \code{pixelGroup} column in \code{cohortData}.}

\item{currentTime}{The current simulation time e.g., \code{time(sim)}.}

\item{speciesEcoregion}{A \code{data.table} with \code{speciesEcoregion} values.}

\item{treedFirePixelTableSinceLastDisp}{A data.table with at least 2 columns, \code{pixelIndex} and \code{pixelGroup}.
This will be used in conjunction with \code{cohortData} and \code{pixelGroupMap}
to ensure that everything matches correctly.}

\item{successionTimestep}{The time between successive seed dispersal events.
In LANDIS-II, this is called "Succession Timestep". This is used here}

\item{cohortDefinitionCols}{the columns in \code{cohortData} that define unique cohorts}

\item{verbose}{Integer, where increasing number is increasing verbosity. Currently,
only level 1 exists; but this may change.}

\item{doAssertion}{A logical indicating whether some internal tests should be run to
ensure the function is running correctly.
Default: \code{getOption("LandR.assertions", TRUE)}.}
}
\value{
A list of length 2, \code{cohortData} and \code{pixelGroupMap}, with
\code{newPixelCohortData} inserted.

A \code{data.table} with a new \code{rbindlist}ed \code{cohortData}
}
\description{
This is a wrapper for  \code{generatePixelGroups}, \code{initiateNewCohort} and updates to
\code{pixelGroupMap} via assignment to new \code{pixelIndex} values in \code{newPixelCohortData}.
By running these all together, there is less chance that they will diverge.
There are some checks internally for consistency.

Calculate new values for \code{B}, add \code{age}, then \code{rbindlist} this
with \code{cohortData}.
}
\details{
Does the following:
\enumerate{
  \item add new cohort data into \code{cohortData};
  \item assign initial \code{B} and \code{age} for new cohort;
  \item assign the new \code{pixelGroup} to the pixels that have new cohort;
  \item update the \code{pixelGroup} map.
}
}
