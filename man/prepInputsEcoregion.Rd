% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/LANDIS-II-tables.R
\name{prepInputsEcoregion}
\alias{prepInputsEcoregion}
\title{Prepare ecoregion table}
\usage{
prepInputsEcoregion(url = NULL, dPath, cacheTags = NULL)
}
\arguments{
\item{url}{If NULL (the default), uses one from the LANDIS-II project:
\url{https://github.com/LANDIS-II-Foundation/Extensions-Succession/master/biomass-succession-archive/trunk/tests/v6.0-2.0/ecoregion.txt"}).}

\item{dPath}{The destination path.}

\item{cacheTags}{User tags to pass to \code{Cache}.}
}
\value{
A \code{data.table}
}
\description{
Get the dummy ecoregion table from LANDIS-II examples.
}
