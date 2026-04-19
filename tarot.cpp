#include <emscripten/fetch.h>
#include <string>
#include <iostream>
#include <cstring>
#include <ctime>
#include <random>

struct TarotCard {
  const char* name;
  const char* vibe;
};

TarotCard const tarot_deck[78] = {
  // Major Arcana.
  {"The Fool", "MAYBE"}, {"The Magician", "YES"}, {"The High Priestess", "MAYBE"},
  {"The Empress", "YES"}, {"The Emperor", "YES"}, {"The Hierophant", "MAYBE"},
  {"The Lovers", "YES"}, {"The Chariot", "YES"}, {"Strength", "YES"},
  {"The Hermit", "MAYBE"}, {"Wheel of Fortune", "MAYBE"}, {"Justice", "MAYBE"},
  {"The Hanged Man", "NO"}, {"Death", "NO"}, {"Temperance", "MAYBE"},
  {"The Devil", "NO"}, {"The Tower", "NO"}, {"The Star", "YES"},
  {"The Moon", "MAYBE"}, {"The Sun", "YES"}, {"Judgement", "YES"},
  {"The World", "YES"},

  // Wands.
  {"Ace of Wands", "YES"}, {"Two of Wands", "MAYBE"}, {"Three of Wands", "YES"},
  {"Four of Wands", "YES"}, {"Five of Wands", "NO"}, {"Six of Wands", "YES"},
  {"Seven of Wands", "MAYBE"}, {"Eight of Wands", "YES"}, {"Nine of Wands", "NO"},
  {"Ten of Wands", "NO"}, {"Page of Wands", "YES"}, {"Knight of Wands", "YES"},
  {"Queen of Wands", "YES"}, {"King of Wands", "YES"},

  // Cups.
  {"Ace of Cups", "YES"}, {"Two of Cups", "YES"}, {"Three of Cups", "YES"},
  {"Four of Cups", "NO"}, {"Five of Cups", "NO"}, {"Six of Cups", "YES"},
  {"Seven of Cups", "MAYBE"}, {"Eight of Cups", "NO"}, {"Nine of Cups", "YES"},
  {"Ten of Cups", "YES"}, {"Page of Cups", "YES"}, {"Knight of Cups", "YES"},
  {"Queen of Cups", "YES"}, {"King of Cups", "YES"},

  // Swords.
  {"Ace of Swords", "YES"}, {"Two of Swords", "MAYBE"}, {"Three of Swords", "NO"},
  {"Four of Swords", "MAYBE"}, {"Five of Swords", "NO"}, {"Six of Swords", "YES"},
  {"Seven of Swords", "NO"}, {"Eight of Swords", "NO"}, {"Nine of Swords", "NO"},
  {"Ten of Swords", "NO"}, {"Page of Swords", "MAYBE"}, {"Knight of Swords", "NO"},
  {"Queen of Swords", "YES"}, {"King of Swords", "MAYBE"},

  // Pentacles.
  {"Ace of Pentacles", "YES"}, {"Two of Pentacles", "MAYBE"}, {"Three of Pentacles", "YES"},
  {"Four of Pentacles", "YES"}, {"Five of Pentacles", "NO"}, {"Six of Pentacles", "YES"},
  {"Seven of Pentacles", "MAYBE"}, {"Eight of Pentacles", "YES"}, {"Nine of Pentacles", "YES"},
  {"Ten of Pentacles", "YES"}, {"Page of Pentacles", "YES"}, {"Knight of Pentacles", "YES"},
  {"Queen of Pentacles", "YES"}, {"King of Pentacles", "YES"}
};

// Callback for when NIST responds.
void download_succeded(emscripten_fetch_t* fetch) {
  std::string data(fetch->data, fetch->numBytes);
  emscripten_fetch_close(fetch);

  std::string search_key = "\"outputValue\" : \"";
  size_t pos = data.find(search_key);

  if (pos == std::string::npos) {
    std::cout << "ERROR: NIST Beacon format changed!!!" << std::endl;
    return;
  }

  // The idea is to extract the first hexadecimal byte. It will
  // represent the answer.
  std::string hexadecimal_byte = data.substr(pos + search_key.length(), 2);

  int value = std::stoi(hexadecimal_byte, nullptr, 16);

  // Modulo 78 for the card (there are 78 cards). We vibing.
  int card_index = value % 78;
  const char* drawn_card = tarot_deck[card_index].name;
  const char* vibe = tarot_deck[card_index].vibe;

  std::cout << "> RAW HEX BYTE: 0x" << hexadecimal_byte << "\n"
            << "> EXTRACTED VALUE: " << value << "\n"
            << "> DRAWN CARD: " << drawn_card << " (" << card_index + 1 << " / 78)\n"
            << "> THE GOVERNMENT SAYS: " << vibe << std::endl;
}

void download_failed(emscripten_fetch_t *fetch)
{
  std::cout << "ERROR: failed to contact NIST servers.\n";
  emscripten_fetch_close(fetch);
}

// We need to expose this function to JavaScript.
extern "C" {
  void consult_oracle()
  {
    std::mt19937 engine(static_cast<unsigned int>(std::time(0)));
    long long start_time = 0;
    long long end_time = std::time(0);
    std::uniform_int_distribution<long long> dist(start_time, end_time);
    long long random_unix_time = dist(engine);

    std::string req = "https://beacon.nist.gov/beacon/2.0/pulse/time/" + std::to_string(random_unix_time);

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = download_succeded;
    attr.onerror = download_failed;

    emscripten_fetch(&attr, req.c_str());
  }
}

int main() {
  std::cout << "[SYSTEM] WASM Runtime Initialised. Awaiting query..." << std::endl;
  return 0;
}
