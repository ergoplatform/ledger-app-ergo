def chunk(array: list, size: int) -> list:
    if not array:
        return []
    if len(array) <= size:
        return [array]

    chunks = []
    for i in range(0, len(array), size):
        chunks.append(array[i:i + size])

    return chunks