from scipy import optimize

from thermal_model.estimation import error, lti_from_data
from thermal_model.logger import LOGGER


def main():
    y, u, t, x0, _ = data

    (A, B, C, _), params = lti_from_data(
        y,
        u,
        t,
        x0,
        optimizer_kwargs={
            "fn": optimize.minimize,
            "method": "L-BFGS-B",
            "jac": "3-point",
            "tol": 1e-3,
            "options": {
                "disp": True,
                "maxiter": 1e2,
            },
            "err": error.l2,
        },
    )
    LOGGER.info("\n")
    LOGGER.info(f"Final parameters: {params}")
    LOGGER.info(f"A = \n{A}")
    LOGGER.info(f"B = \n{B}")
    LOGGER.info(f"C = \n{C}")
    print(params)
