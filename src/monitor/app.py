import csv
import datetime

import serial

from logger import LOGGER, setup_logger


def parse_line(line):
    return line.split(",")


def start(out_dir, port, baud_rate=9600, timeout=5, *, log_dir=False, debug=False, skip_header=False):
    setup_logger(log_dir=log_dir, debug=debug, verbose=True)

    LOGGER.info("Starting communication to port '%s'", port)
    ser = serial.Serial(port, baud_rate, timeout=timeout)

    LOGGER.info("Opening csv file '%s'", out_dir)
    with open(out_dir, "w", newline="") as file:
        writer = csv.writer(file)
        if not skip_header:
            writer.writerow(["timestamp", "unix_time", "resistance", "temperature"])

        LOGGER.info("Entering communication loop")
        while ser.is_open:
            line = ser.readline()

            if not line:
                LOGGER.info("Ending communication to port '%s'", port)
                break
            else:
                LOGGER.debug(line)
                timestamp = datetime.datetime.now(datetime.UTC)
                unix = int((timestamp - datetime.datetime(1970, 1, 1, tzinfo=datetime.UTC)).total_seconds())
                writer.writerow([timestamp, unix, *parse_line(line)])


start("asdfsadf.csv", "COM1", debug=True)
