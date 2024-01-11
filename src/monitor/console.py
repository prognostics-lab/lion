import dataclasses


@dataclasses.dataclass
class ShellColors:
    """Colors for text in the console"""

    GREY = "\x1b[38;20m"
    CYAN = "\x1b[36;20m"
    YELLOW = "\x1b[33;20m"
    GREEN = "\x1b[32;20m"
    RED = "\x1b[31;20m"
    BOLD_RED = "\x1b[31;1m"
    RESET = "\x1b[0m"

    BG_MAGENTA = "\x1b[45m"
    BG_RESET = "\x1b[49m"
