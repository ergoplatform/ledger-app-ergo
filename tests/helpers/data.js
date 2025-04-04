const { Address, NetworkPrefix, DerivationPath, ExtPubKey } = require("ergo-lib-wasm-nodejs");
const { toBytes } = require("./common");

class Account {
    constructor(publicKey, chainCode, path) {
        this.publicKey = ExtPubKey.new(toBytes(publicKey), toBytes(chainCode), path);
        this.path = path;
    }
}

class ExtendedAddress {
    constructor(network, address, path) {
        this.network = network;
        this.address = address;
        this.path = DerivationPath.new(path[0], [path[1]]);
        this.acc_index = path[0];
        this.addr_index = path[1];
    }

    toBase58() {
        return this.address.to_base58(this.network);
    }

    toBytes() {
        return this.address.to_bytes(this.network);
    }
}

class TestData {
    constructor(network) {
        this.network = network;
        this.account = new Account(
            '03c24e55008b523ccaf03b6c757f88c4881ef3331a255b76d2e078016c69c3dfd4',
            '8eb29c7897d57aee371bf254be6516e6963e2d9b379d0d626c17a39d1a3bf553',
            DerivationPath.from_string(`m/44'/429'/0'`)
        );
        this.address0 = new ExtendedAddress(
            network,
            Address.from_base58("9gqBSpseifxnkjRLZUxs5wbJGsvYPG7MLRcBgnKEzFiJoMJaakg"),
            [0, 0]
        );
        this.address1 = new ExtendedAddress(
            network,
            Address.from_base58("9iKPzGpzrEFFQ7kn2n6BHWU4fgTwSMF7atqPsvHAjgGvogSHz6Y"),
            [0, 1]
        );
        this.changeAddress = new ExtendedAddress(
            network,
            Address.from_base58("9eo8hALVQTaAuu8m95JhR8rhXuAMsLacaxM8X4omp724Smt8ior"),
            [0, 2]
        );
        this.changeAddress22 = new ExtendedAddress(
            network,
            Address.from_base58("9fRejXDJxxdJ1KVRH6HdxDj1S1duKmUNGG7CjztN2YjHnooxYAX"),
            [0, 22]
        );
        this.addressScript = new ExtendedAddress(
            network,
            Address.from_base58("HTNVLC7W3rw7LbSXR1biLQqQgmdz9d9D1Wpwt73WmkEVcxMpQzzajV6njQQcET9ytvR2ZALwtr1jn3DLAKPs8FSiMUbU9XLvY8BrQYRnjLiiF58EYgDGa72tLugcLWbh84S1cuYkFebZaH8Ataoiu6ssF9eVPhJeaYqXxKEqKnR89HfGZBk48phyoYvWR4ZyKkm65DNAsxHz7Z2GELjAkAbuX3xobgzao5MZeLb7nDxemNBSoBhRz3n34wd4eVvKeoG5KEkax7g1vGWhMGajHh5QPVpc62H4cYopnEeoEg18FM82QiHjhSPJy84RvBhGa3912PRJgYDKxvnGVgTzG7R3uePoBeypGExCaG4YQHZ6caH7aDBBqZkVK9hg3tzXYd7XCj6jqTGkFRsdMuvkC2nJBJjzzi1txKpcPFLqdr5Tcm7BufPmJUz1zSmtVHzuT9DLBDYwuRkrVZG15mWZqP9Kyq5DqoEgA6GXUJJuRv7HrpBQG6U1oGHdAgiWqPo7qx679tVSjE63wzB1sphJ9GNxQCTTXJ79ZonGDfbmxGzL3Raoq7YsFJKZoNNgZDsdpYzcMJjvxj4FVTcmzC5StAHAvQkkWuX5mUkjF18DYt8ZQ52GThWPznHEGLvGku9ryiNPWbhEca5izzhH8mLjykYcqfTyZ7cVnQk46q31zyG3MsXXewb9bFDDk4deB6LCaQcA"),
            [0, 0]
        );
        this.addressScriptHash = "72H58ETmT5dN7GfQnHbAfo2WLPuGxcpoP6QkQJn";
    }
}

exports.TEST_DATA = new TestData(NetworkPrefix.Mainnet);
