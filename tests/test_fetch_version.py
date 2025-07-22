from ragger.backend.interface import BackendInterface

from application_client.ergo_command_sender import ErgoCommandSender
from application_client.ergo_response_unpacker import unpack_get_version_response
from utils import verify_version

def test_fetch_version(backend: BackendInterface) -> None:
    client = ErgoCommandSender(backend)
    rapdu = client.get_version()
    MAJOR, MINOR, PATCH, DEBUG = unpack_get_version_response(rapdu.data)
    verify_version(f"{MAJOR}.{MINOR}.{PATCH}")