#include "../include/tabular.hpp"

int main() {
    using namespace tabular;

    Table table;
    table.addRow({"Country", "Capital"});
    table.addRow({"Canada", "Ottawa"});
    table.addRow({"Peru", "Lima"});
    table.addRow({"United States", "Washington"});
    table.addRow({"Brazil", "Brasília"});

    table.printTable();
    return 0;
}