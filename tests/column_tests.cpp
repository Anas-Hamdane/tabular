#include "gtest/gtest.h"
#include "../include/tabular/column.h"

// to avoid repeating
using namespace tabular;

TEST(column_tests, basic_formatting)
{
  Column column("Hello, World");
  column.config().width(15);

  auto lines = column.lines();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], " Hello, World  ");

  column.config().align(Alignment::Center);
  lines = column.lines();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], " Hello, World  ");

  column.config().align(Alignment::Right);
  lines = column.lines();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], "  Hello, World ");
}

TEST(column_tests, padding_formatting)
{
  Column column("Hello, World");
  column.config().width(22);

  column.config().padd(Padding(0, 5));
  auto lines = column.lines();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], "     Hello, World     ");

  column.config().padd(Padding(1, 5));
  lines = column.lines();
  ASSERT_EQ(lines.size(), 3);
  EXPECT_EQ(lines[0], "                      ");
  EXPECT_EQ(lines[1], "     Hello, World     ");
  EXPECT_EQ(lines[2], "                      ");

  column.config().width(32);
  column.config().padd(Padding(2, 10));
  lines = column.lines();
  ASSERT_EQ(lines.size(), 5);
  EXPECT_EQ(lines[0], "                                ");
  EXPECT_EQ(lines[1], "                                ");
  EXPECT_EQ(lines[2], "          Hello, World          ");
  EXPECT_EQ(lines[3], "                                ");
  EXPECT_EQ(lines[4], "                                ");
}

TEST(column_tests, styling)
{
  Column column("Hello, World");
  column.config().width(15);

  std::vector<std::string> lines;

  column.style().fg(Rgb(255, 0, 0));
  lines = column.lines();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], " \x1b[38;2;255;0;0mHello, World\x1b[0m  ");

  column.style().fg(Color::Red);
  lines = column.lines();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], " \x1b[31mHello, World\x1b[0m  ");

  column.style().clrFg();

  column.style().bg(Rgb(0, 255, 0));
  lines = column.lines();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], " \x1b[48;2;0;255;0mHello, World\x1b[0m  ");

  column.style().bg(Color::Green);
  lines = column.lines();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], " \x1b[42mHello, World\x1b[0m  ");

  column.style().clrBg();

  column.style().base(Rgb(0, 0, 255));
  lines = column.lines();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], "\x1b[48;2;0;0;255m Hello, World  \x1b[0m");

  column.style().base(Color::Blue);
  lines = column.lines();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], "\x1b[44m Hello, World  \x1b[0m");

  column.style().clrBase();

  column.style().attrs(Attribute::Bold | Attribute::Italic | Attribute::Underline);
  lines = column.lines();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], " \x1b[1;3;4mHello, World\x1b[0m  ");

  column.style().clrAttrs();

  // merged
  column.style().fg(Color::Red);
  column.style().bg(Color::Green);
  column.style().base(Color::Blue);
  column.style().attrs(Attribute::Bold | Attribute::Underline);
  lines = column.lines();
  ASSERT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], "\x1b[44m \x1b[1;4;31;42mHello, World\x1b[0m\x1b[44m  \x1b[0m");
}

TEST(column_tests, nested_styles)
{
  Column column("Hello, World!! \x1b[31mLet's start the journey\x1b[0m");
  column.config().width(16); // multiline

  column.style().fg(Color::Cyan);
  auto lines = column.lines();

  ASSERT_EQ(lines.size(), 3);
  EXPECT_EQ(lines[0], " \x1b[36mHello, World!!\x1b[0m ");
  EXPECT_EQ(lines[1], " \x1b[36m\x1b[31mLet's start \x1b[0m   ");
  EXPECT_EQ(lines[2], " \x1b[36m\x1b[31mthe journey\x1b[0m    ");
}