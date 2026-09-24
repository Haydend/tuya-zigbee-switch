# Unit Tests

Run from the project root. Test tools (Unity, CMock) are downloaded automatically on first run.

## Run all tests

```
make unittest/test/all
```

## Run all tests in a file

```
make unittest/test/test_push_button
```

## Run filtered tests

Uses substring matching on test function names:

```
make unittest/test/test_push_button RUN_TEST=test_pin_goes_low
```

## Interactive debugging with VS Code

Requires the [CodeLLDB extension](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb).

1. Set breakpoints by clicking the gutter next to a line number
2. Open the Run and Debug panel (Cmd+Shift+D)
3. Select a configuration:
   - **Debug Unit Test** — runs all tests in a file with the debugger attached
   - **Debug Unit Test (filtered)** — prompts for a test function name to filter to
4. Press F5 to launch
