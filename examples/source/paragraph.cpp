/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane
*/

#include <tabular/printer.hpp>
// #include <tabular/tabular.hpp>

int main() {
  using namespace tabular;

  Table table;
  table.add_row({"English", "French", "Japanese"})
      .add_row({
          "The wind whispered through the trees,\ncarrying the scent of rain "
          "and earth.\nA lone traveler walked the forest path,\nlost in "
          "thought, guided by moonlight.",

          "Le vent murmurait à travers les arbres,\nportant l’odeur de la "
          "pluie et de la terre.\nUn voyageur solitaire marchait sur le "
          "sentier,\nperdu dans ses pensées, guidé par la lune.",

          "風が木々を通り抜けてささやいた。\n雨と土の香りを運んでいた。\nひとり"
          "の旅人が森の道を歩いていた。\n月明かりに導かれながら、物思いにふけっ"
          "ていた。",
      });

  // !IMPORTANT
  table.set().multi_byte_characters(true).width_percent(100); // 100% of the terminal width

  table.border().set().style(BorderStyle::ansi);

  print(table);
  return 0;
}
