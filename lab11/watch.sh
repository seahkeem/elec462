#!bin/sh
#
#   watch.sh - a simple version of the watch utility, written in sh
#

        who | sort > prev
        while true
        do
          sleep 10
          who | sort > current
          echo "Logged out:"
          comm -23 prev current
          echo "Logged in:"
          comm -13 prev current
          mv current prev
        done
