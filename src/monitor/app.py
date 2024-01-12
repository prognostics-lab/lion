import csv
import datetime

import serial

from monitor.logger import LOGGER, setup_logger


def parse_line(line):
    decoded = line.decode()
    if decoded.endswith("\n"):
        decoded = decoded[:-1]
    if decoded.endswith("\r"):
        decoded = decoded[:-1]
    decoded = decoded.split(",")
    decoded[1] = str(int(decoded[1]) - 273)
    return decoded


def start(out_dir, port, baud_rate=9600, timeout=5, *, skip_header=False):
    LOGGER.info("Starting communication to port '%s', br%i", port, baud_rate)
    ser = serial.Serial(port, baud_rate, timeout=timeout)

    LOGGER.info("Opening csv file '%s'", out_dir)
    with open(out_dir, "w", newline="") as file:
        writer = csv.writer(file)
        if not skip_header:
            writer.writerow(["timestamp", "unix_time", "resistance", "temperature_celsius"])

        LOGGER.info("Entering communication loop")
        while ser.is_open:
            try:
                line = ser.readline()

                if not line:
                    LOGGER.info("Ending communication to port '%s'", port)
                    break
                else:
                    LOGGER.debug(line)
                    timestamp = datetime.datetime.now(datetime.UTC)
                    unix = int((timestamp - datetime.datetime(1970, 1, 1, tzinfo=datetime.UTC)).total_seconds())

                    parsed = parse_line(line)
                    if len(parsed) != 2:
                        LOGGER.warning("Found invalid line: '%s'", line)
                        continue
                    writer.writerow([timestamp, unix, *parsed])
            except Exception as e:
                LOGGER.warning("Found exception '%s'", e)
                ser.close()
