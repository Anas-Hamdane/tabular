#include "gtest/gtest.h"
#include "../include/tabular/column.h"

// to avoid repeating
using namespace tabular;

TEST(column_tests, basic_formatting)
{
  Column column("Hello, World");
  column.config().width(15);

  std::vector<String> lines = column.toString();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0].toStr(), " Hello, World  ");

  column.config().align(Alignment::Center);
  lines = column.toString();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0].toStr(), " Hello, World  ");

  column.config().align(Alignment::Right);
  lines = column.toString();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0].toStr(), "  Hello, World ");
}

TEST(column_tests, padding_formatting)
{
  Column column("Hello, World");
  column.config().width(22);

  column.config().padd(Padding(0, 5));
  std::vector<String> lines = column.toString();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0].toStr(), "     Hello, World     ");

  column.config().padd(Padding(1, 5));
  lines = column.toString();
  ASSERT_EQ(lines.size(), 3);
  EXPECT_EQ(lines[0].toStr(), "                      ");
  EXPECT_EQ(lines[1].toStr(), "     Hello, World     ");
  EXPECT_EQ(lines[2].toStr(), "                      ");

  column.config().width(32);
  column.config().padd(Padding(2, 10));
  lines = column.toString();
  ASSERT_EQ(lines.size(), 5);
  EXPECT_EQ(lines[0].toStr(), "                                ");
  EXPECT_EQ(lines[1].toStr(), "                                ");
  EXPECT_EQ(lines[2].toStr(), "          Hello, World          ");
  EXPECT_EQ(lines[3].toStr(), "                                ");
  EXPECT_EQ(lines[4].toStr(), "                                ");
}

TEST(column_tests, styling)
{
  Column column("Hello, World");
  column.config().width(15);

  std::vector<String> lines;

  column.style().fg(Rgb(255, 0, 0));
  lines = column.toString();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0].toStr(), " \x1b[38;2;255;0;0mHello, World\x1b[0m  ");

  column.style().fg(Color::Red);
  lines = column.toString();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0].toStr(), " \x1b[31mHello, World\x1b[0m  ");

  column.style().resetFg();

  column.style().bg(Rgb(0, 255, 0));
  lines = column.toString();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0].toStr(), " \x1b[48;2;0;255;0mHello, World\x1b[0m  ");

  column.style().bg(Color::Green);
  lines = column.toString();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0].toStr(), " \x1b[42mHello, World\x1b[0m  ");

  column.style().resetBg();

  column.style().base(Rgb(0, 0, 255));
  lines = column.toString();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0].toStr(), "\x1b[48;2;0;0;255m Hello, World  \x1b[0m");

  column.style().base(Color::Blue);
  lines = column.toString();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0].toStr(), "\x1b[44m Hello, World  \x1b[0m");

  column.style().resetBase();

  column.style().attrs(Attribute::Bold | Attribute::Italic | Attribute::Underline);
  lines = column.toString();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0].toStr(), " \x1b[1;3;4mHello, World\x1b[0m  ");

  column.style().resetAttrs();

  // merged
  column.style().fg(Color::Red);
  column.style().bg(Color::Green);
  column.style().base(Color::Blue);
  column.style().attrs(Attribute::Bold | Attribute::Underline);
  lines = column.toString();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0].toStr(), "\x1b[44m \x1b[1;4;31;42mHello, World\x1b[0m\x1b[44m  \x1b[0m");
}

TEST(column_tests, nested_styles)
{
  Column column("Hello, World!! \x1b[31mLet's start the journey\x1b[0m");
  column.config().width(16); // multiline

  column.style().fg(Color::Cyan);
  auto lines = column.toString();

  ASSERT_EQ(lines.size(), 3);
  EXPECT_EQ(lines[0].toStr(), " \x1b[36mHello, World!!\x1b[0m ");
  EXPECT_EQ(lines[1].toStr(), " \x1b[36m\x1b[31mLet's start \x1b[0m   ");
  EXPECT_EQ(lines[2].toStr(), " \x1b[36m\x1b[31mthe journey\x1b[0m    ");
}