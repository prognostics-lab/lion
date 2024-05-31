from thermal_model.estimation import error


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
            "tol": 1e-3,
            "options": {
                "maxiter": 1e2,
            },
            "err": error.l2,
        },
    )
    logger.info("\n")
    logger.info(f"Final parameters: {params}")
    logger.info(f"A = \n{A}")
    logger.info(f"B = \n{B}")
    logger.info(f"C = \n{C}")
    print(params)
    return A, B, C, params, data
