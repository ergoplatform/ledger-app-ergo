from struct import unpack
from typing import Tuple


def unpack_get_version_response(response: bytes) -> Tuple[int, int, int]:
    assert len(response) == 4
    major, minor, patch, debug = unpack("BBBB", response)
    return (major, minor, patch, debug)

def unpack_get_app_name_response(response: bytes) -> str:
    return response.decode("ascii")