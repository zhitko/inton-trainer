# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [0.4.10] - 2019-03-28

### Add
- New autosegmentation algorithm by custom vectors
- Add keep ratio to UMP/NMP generation
- Add automarking switch to graph window

### Change
- Change show original/nmp to 2 button switch
- Chnage graph window buttons titles
- Change calculation derivative gravity center

## Fix
- Automarking issue with nmp signal zero value
- Settings changes apply
- UMP segmant scale to zero

## [0.4.9] - 2019-03-05

### Fix
- Fix play Record Button title to Play Template
- Fix METRIC_MEAN_VALUE_UMP metrics calculation

## [0.4.8] - 2019-02-18

### Add
- Show devivative mean value

### Fix
- Show derivative graph mean and center in derivative mod
- Calculating of center on gravity center

## [0.4.7] - 2019-02-15

### Add
- Show devivative gravity center UMP

## [0.4.6] - 2019-02-11

### Add
- Save metrics for single file
- Add new metrics

### Change
- Save relative metrics as 10 * log10

## [0.4.5] - 2019-02-07

### Fix
- Fix gravity center UMP calculation

### Add
- Add gravity center UMP length metric

## [0.4.4] - 2019-02-05

### Change
- Replace gravity center UMP to partial gravity certer UMP 1 and 2

## [0.4.3] - 2019-01-22

### Add
- add new metrics to xslx file to section "Reference data on templates and records"

## [0.4.2] - 2019-01-21

### Change
- add xslx formatted output

## [0.4.1] - 2019-01-18

### Add
- add gravity center plot
- add mean value plot
- add new metrics saved to file
- add open saved file metrics

## [0.4.0] - 2019-01-14

### Add
- Save file comparsion mertics to xslx file

## [0.3.8] - 2018-12-18

### Change
- move display and autosegment setting to first screen
- add to phase plate ticks
- change display text for UMP similarity
- change open instance file button position
- change phase plote size
- add phase plot legend

## [0.3.6] - 2018-12-16

### Add
- Add phase plate view for F0 and derivative F0

### Fix
- Fix marks calculation on single derivative F0 mode

## [0.3.5] - 2018-12-12

### Add
- Add separate devative UMP graph

## [0.3.4] - 2018-12-10

### Add
- Add UMP based on derivative F0

## [0.3.3] - 2018-12-09

### Add
- Add derivative F0 graph with it's zero level

## [0.3.2] - 2018-11-20

### Change
- Change relative limit formula to max and min values in segment

## [0.3.1] - 2018-11-03

### Add
- Add relative auto segmentation

## [0.3.0] - 2018-10-29

### Add
- Add automarking mode

## [0.2.13] - 2018-10-11

### Changed
- Smooth for A0 changed to liner

## [0.2.12] - 2018-09-03

### Changed
- Autosegmentation by F0&A0 set as default
- Add UMP vertical delimeters

### Added
- Segmentation pattern

### Fixed
- Fixed save wave file with correct file size in header

## [0.2.11] - 2018-07-16

### Added
- Added autosegmentation by A0 and F0&A0

### Fixed
- Fixed A0 calculation

## [0.2.10] - 2018-05-13

### Added
- Added "Cepstrum with A0 or/and A0'" for DP processing

## [0.2.9] - 2018-05-03

### Changed
- Advanced settings refactored

### Added
- Added "Cepstrum with A0 or/and logF0" for DP processing

## [0.2.8] - 2018-03-11

### Fixed
- Fix first block interpolation
- Fix mark out algorithm
- Fix mark history files

### Changed
- Markout files prefix to underscore
- Change graph A to blue

## [0.2.7] - 2018-03-04

### Fixed
- Fix cut pitch by mask and LogF0
- Fix interpolation in firts UMP part

## [0.2.6] - 2018-02-27

### Fixed
- Fix auto mark function

## [0.2.5] - 2018-02-25

### Fixed
- Fixed F0 interpolation processing

### Removed
- Removed calculation type for F0 curves - relative
- Removed calculation type for F0 curves - average relative

### Added
- New calculation type for F0 curves - average rating

## [0.2.4] - 2018-02-18

### Changed
- Changed range mark calculation

### Added
- New calculation type for F0 curves - relative
- New calculation type for F0 curves - average relative

## [0.2.3] - 2018-02-10

### Changed
- Settings window move Similarity Measure Calculation to main settings

### Removed
- tab 'more' from graph window
- Remove unused settings

## [0.2.2] - 2018-02-09

### Changed
- Settings window changed titles and labels
- Graph window changed titles and labels

## [0.2.1] - 2018-02-04

### Added
- Add templates autoprocess
- Add 'Open File Dir' to template database actions

## [0.2.0] - 2018-01-27

### Added
- Template databse editor

## [0.1.5] - 2018-01-24

### Added
- Set bold font for selected error

### Changed
- change range calculation to "Pd = 100% * Min (Rt, Ru) / Max (Rt, Ru)"

### Removed
- field 'Proximity to curve shape' from graph window

## [0.1.4] - 2018-01-21

### Added
- Show school marks settings
- Calculation of Local proximity F0 curve added to available fomulas in settings

### Changed
- Change settings title from error to proximity

### Removed
- remove calculation of Similarity of F0-curve

## [0.1.3] - 2018-01-19

### Changed
- Change settings Err title to Prox
- Fix fault on render graph with error translation

## [0.1.2] - 2018-01-17

### Changed
- Result marks with it's percentage

### Removed
- Average result mark from graph window

## [0.1.1] - 2018-01-16

### Added
- Add wave smoothing before F0 calculating

### Changed
- Proximity to curve calculate by correlation

## [0.1.0] - 2018-01-14

### Added
- Added changelog file
- Start application versions
- Add marks settings (limit, delimeter and labels)

### Changed
- Result marks changed from percents to 5 level marks
- Recording time settings changed to double

## [0.0.0] - 2014-04-18

### Added
-

### Changed
-

### Removed
-
