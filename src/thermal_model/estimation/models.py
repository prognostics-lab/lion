import numpy as np
from scipy import signal

from thermal_model.estimation.params import TargetParams, TargetNLParams


def target_lti_parameters(params: TargetParams | TargetNLParams):
    """Returns the A, B, C, D matrices of the target LTI system"""
    rt = params.rin + params.rout
    a_mat = np.array([
        [-1 / (params.cp * rt), 1 / (params.cp * rt)],
        [1 / (params.cair * rt), -(params.rair + rt) / (params.cair * params.rair * rt)],
    ])
    b_mat = np.array([
        [0, 1 / params.cp],
        [1 / (params.cair * params.rair), 0],
    ])
    c_mat = np.array([
        [params.rout / rt, params.rin / rt],
        [0, 1],
    ])
    d_mat = np.zeros((2, 2))
    return a_mat, b_mat, c_mat, d_mat


def target_lti_parameters_sftemp(params: TargetParams | TargetNLParams):
    """Returns the A, B, C, D matrices of the target LTI system"""
    rt = params.rin + params.rout
    a_mat = np.array([
        [-1 / (params.cp * rt), 1 / (params.cp * rt)],
        [1 / (params.cair * rt), -(params.rair + rt) / (params.cair * params.rair * rt)],
    ])
    b_mat = np.array([
        [0, 1 / params.cp],
        [1 / (params.cair * params.rair), 0],
    ])
    c_mat = np.array([
        [params.rout / rt, params.rin / rt],
    ])
    d_mat = np.zeros((1, 2))
    return a_mat, b_mat, c_mat, d_mat


def target_lti_parameters_airtemp(params: TargetParams | TargetNLParams):
    """Returns the A, B, C, D matrices of the target LTI system"""
    rt = params.rin + params.rout
    a_mat = np.array([
        [-1 / (params.cp * rt), 1 / (params.cp * rt)],
        [1 / (params.cair * rt), -(params.rair + rt) / (params.cair * params.rair * rt)],
    ])
    b_mat = np.array([
        [0, 1 / params.cp],
        [1 / (params.cair * params.rair), 0],
    ])
    c_mat = np.array([
        [0, 1],
    ])
    d_mat = np.zeros((1, 2))
    return a_mat, b_mat, c_mat, d_mat


def get_lti_params_fn(outputs: {"sf", "air", "both"} = "both"):
    match outputs:
        case "sf":
            return target_lti_parameters_sftemp
        case "air":
            return target_lti_parameters_airtemp
        case "both":
            return target_lti_parameters
        case _:
            print(f"**WARNING** Option '{outputs}' not recognized, defaulting to 'both'")
            return target_lti_parameters


def target_lti(params: TargetParams | TargetNLParams, outputs: {"sf", "air", "both"} = "both", **_) -> signal.StateSpace:
    """Returns an LTI StateSpace instance for the corresponding values"""
    params_fn = get_lti_params_fn(outputs)
    return signal.lti(*params_fn(params))


def target_response_noisy(u, t, x0, params: TargetParams, chamber_std, cell_std, **kwargs):
    sys = target_lti(params, **kwargs)
    model_noise = np.random.normal(0, chamber_std, u.shape)
    y = sys.output(u + model_noise, t, x0)[1]
    sensor_noise = np.random.normal(0, cell_std, y.shape)
    return y + sensor_noise


def target_response(u, t, x0, params: TargetParams, **kwargs):
    sys = target_lti(params, **kwargs)
    out = sys.output(u, t, x0)
    y = out[1]
    x = out[2]
    return y


def target_response_noisy_nl(u, t, x0, params: TargetNLParams, chamber_std, cell_std, **kwargs):
    sys = target_lti(params, **kwargs)
    model_noise = np.random.normal(0, chamber_std, u.shape)
    y = sys.output(u + model_noise, t, x0)[1] + params.k
    sensor_noise = np.random.normal(0, cell_std, y.shape)
    return y + sensor_noise


def target_response_nl(u, t, x0, params: TargetNLParams, **kwargs):
    sys = target_lti(params, **kwargs)
    out = sys.output(u, t, x0)
    y = out[1] + params.k
    x = out[2]
    return y


# def generate_l1_error(expected, u, t, x0, chamber_std, cell_std, **kwargs):
#     def _generate_system(params: TargetParams) -> float:
#         obtained = target_response_noisy(u, t, x0, params, chamber_std, cell_std, **kwargs)
#         error = expected - obtained
#         return np.abs(error).sum() / len(t)
#     return _generate_system


# def generate_l2_error(expected, u, t, x0, chamber_std, cell_std, **kwargs):
#     def _generate_system(params: TargetParams) -> float:
#         obtained = target_response_noisy(u, t, x0, params, chamber_std, cell_std, **kwargs)
#         error = expected - obtained
#         try:
#             mse = np.diag(error.conjugate().T @ error).sum() / len(t)
#         except ValueError:
#             mse = error.conjugate().T @ error / len(t)
#         return mse
#     return _generate_system


# def generate_l1_error_nl(expected, u, t, x0, chamber_std, cell_std, **kwargs):
#     def _generate_system(params: TargetNLParams) -> float:
#         obtained = target_response_noisy_nl(u, t, x0, params, chamber_std, cell_std, **kwargs)
#         error = expected - obtained
#         return np.abs(error).sum() / len(t)
#     return _generate_system


# def generate_l2_error_nl(expected, u, t, x0, chamber_std, cell_std, **kwargs):
#     def _generate_system(params: TargetNLParams) -> float:
#         obtained = target_response_noisy_nl(u, t, x0, params, chamber_std, cell_std, **kwargs)
#         error = expected - obtained
#         return np.diag(error.conjugate().T @ error).sum() / len(t)
#     return _generate_system


def generate_evaluation(expected, u, t, x0, **kwargs):
    def _generate_system(params: TargetParams) -> float:
        y, x = target_response(u, t, x0, params, **kwargs)
        error = expected - y
        return expected, y, error, x
    return _generate_system


def generate_evaluation_nl(expected, u, t, x0, **kwargs):
    def _generate_system(params: TargetNLParams) -> float:
        y, x = target_response_nl(u, t, x0, params, **kwargs)
        error = expected - y
        return expected, y, error, x
    return _generate_system
