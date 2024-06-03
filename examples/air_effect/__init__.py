from .estimate_parameters import main as _estimate_parameters_main
from .plot_results import main as _plot_results_main


def main(plot=True):
    if plot:
        _plot_results_main()
    else:
        _estimate_parameters_main()
