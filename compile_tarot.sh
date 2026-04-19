# em++ tarot.cpp -o tarot_engine.js -s FETCH=1 -s EXPORTED_FUNCTIONS='["_consult_oracle"]' -s EXPORTED_RUNTIME_METHODS='["ccall"]'
em++ tarot.cpp -o tarot_engine.js -s FETCH=1 -s EXPORTED_FUNCTIONS='["_main", "_consult_oracle"]' -s EXPORTED_RUNTIME_METHODS='["ccall"]' -s NO_EXIT_RUNTIME=1
