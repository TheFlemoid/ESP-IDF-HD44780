# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Released - 17FEB25]
### Added
### ESP-IDF-HD44780 v1.0
- Added support for four row displays.
- Added support for 20 column displays.
  - HD44780_BUS objects now take params indicating the number of columns and rows
    of the display that is being used.
- Added SNTP examples that connect to wifi and use the HD44780 as an NTP display.
  - SNTP examples were included for both common 2x16 as well as 4x20 displays.

## [Prerelease - 12FEB25]
### Added
### ESP-IDF-HD44780 v0.9
- Initial release of library.  Library has full 4-bit and 8-bit HD44780 functionality, 
  and is set up to be used as an ESP-IDF Component for arbitrary projects.  Still have
  a couple of examples to add before I call this a v1.0 release of the library.
