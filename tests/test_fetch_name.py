from ragger.backend.interface import BackendInterface

from application_client.ergo_command_sender import ErgoCommandSender
from application_client.ergo_response_unpacker import unpack_get_app_name_response
from utils import verify_name

def test_fetch_name(backend: BackendInterface) -> None:
    client = ErgoCommandSender(backend)
    rapdu = client.get_name()
    NAME = unpack_get_app_name_response(rapdu.data)
    verify_name(NAME)