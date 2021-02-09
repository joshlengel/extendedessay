#!/usr/bin/expect -f

set timeout 300

set year_start [lindex $argv 0]
set year_end [lindex $argv 1]

array set output_files {
  10 null
  1 null
  2 null
  399 null
  301 null
  4 null
  5 null
  6 null
  7 null
  8 null
}

array set output_files_data {
  10 null
  199 null
  299 null
  399 null
  301 null
  499 null
  599 null
  699 null
  799 null
  899 null
}

spawn telnet ssd.jpl.nasa.gov 6775

proc load_body_data { b } {
  expect "Horizons>"
  send "$b\n"

  expect {
    " < Scroll" {
      send "q"
      exp_continue
    }

    "Select" {
      send "F\n"
    }
  }

  expect -re { Full path\s*:\s*(.*?)\s*\r?\n }
  global output_files_data
  set output_files_data($b) $expect_out(1,string)

  send "\n"
}

proc load_body { b } {
  expect "Horizons>"
  send "$b\n"

  expect {
    " < Scroll" {
      send "q"
      exp_continue
    }

    "Select" {
      send "E\n"
    }
  }

  expect "Observe"
  send "v\n"

  expect {
    "Use previous" {
      send "y\n"
    }

    "Coordinate" {
      send "@SSB\n"
    }
  }

  expect "Reference"
  send "eclip\n"

  expect "Starting TDB"
  send "$::year_start-Jan-1 00:00\n"

  expect "Ending "
  send "$::year_end-Jan-1 00:00\n"

  expect "Output"
  send "1h\n"

  expect "Accept default"
  send "n\n"

  expect "Output ref"
  send "ICRF\n"

  expect "Corrections"
  send "1\n"

  expect "Output units"
  send "1\n"

  expect "Spreadsheet CSV"
  send "YES\n"

  expect "Output delta-T"
  send "NO\n"

  expect "Select output"
  send "2\n"

  expect {
    " < Scroll" {
      send "q"
      exp_continue
    }

    ">>> Select" {
      send "F\n"
    }
  }

  expect -re { Full path\s*:\s*(.*?)\s*\r?\n }
  global output_files
  set output_files($b) $expect_out(1,string)

  expect ">>> Select"
  send "N\n"
}

foreach idx [array names output_files_data] {
  load_body_data $idx
}

foreach idx [array names output_files] {
  load_body $idx
}

send "exit"

spawn bash

puts "Downloading data files ..."
expect "$ "
foreach idx [array names output_files_data] {
  puts "$idx:"
  send "curl -u anonymous:jlengel12@gmail.com $output_files_data($idx) -o ./assets/body-data-$idx.txt\n"
  expect "$ "
}

puts "Downloading ephemerides files ..."
foreach idx [array names output_files] {
  puts "$idx:"
  send "curl -u anonymous:jlengel12@gmail.com $output_files($idx) -o ./assets/body-$idx.csv\n"
  expect "$ "
}
