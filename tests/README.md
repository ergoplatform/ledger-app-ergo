# End-to-end tests

These tests are implemented in NodeJS and can be executed either using the [Speculos](https://github.com/LedgerHQ/speculos) emulator or a Ledger Nano S+/X.
NodeJS dependencies are listed in [package.json](package.json), install them using `npm` or `yarn`.

```
npm install
```

### Launch with Speculos

First start your application with Speculos

```
export SEED=`cat '/path/to/tests/seed.txt'`
./path/to/speculos.py /path/to/bin/app.elf --apdu-port 9999 --seed "$SEED"
```

then run

```
npm run test --model=<model>
```

### Launch with your Nano S+/X

Be sure to have you device connected through USB (without any other software interacting with it) and run

```
LEDGER_LIVE_HARDWARE=1 npm run test --model=<model>
```

`<model>` can be either `nanox` or `nanosp`