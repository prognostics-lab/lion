import matplotlib.pyplot as plt

from thermal_model.logger import LOGGER

from .estimate_parameters import main as main_estimate
from .plot_results import main as main_plot


def main(estimate=True, plot=True, show=False, save=False):
    if estimate:
        LOGGER.info("Running parameter estimation")
        main_estimate()
    else:
        LOGGER.warning("Skipping parameter estimation")

    if plot:
        LOGGER.info("Running plotting of estimated parameters")
        main_plot(save)
    else:
        LOGGER.warning("Skipping plotting of estimated parameters")

    if show:
        plt.show()
