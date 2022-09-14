/*
MIT License

Copyright (c) 2022 Roblox Research

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <gtest/gtest.h>
#include <SIMDString.h>
#include <string>

char sampleString[44] = "the quick brown fox jumps over the lazy dog";

TEST(SIMDStringTest, Construct)
{
  SIMDString simdstring0;
  SIMDString simdstring1('a');
  SIMDString simdstring2("0123456789abcdefghijklmnopqrstuvwxyz");
  SIMDString simdstring3(simdstring2, 10);
  SIMDString simdstring4(simdstring2, 10, 10);
  SIMDString simdstring5(simdstring2.begin(), simdstring2.begin() + 10);
  SIMDString simdstring6{SIMDString(sampleString)};
  SIMDString simdstring7({'a', 'b', 'c'});

  std::string string1(sampleString);
  SIMDString simdstring8(string1, 40);
  SIMDString simdstring9(string1, 4, 5);
  SIMDString simdstring10(10, 'b');
  SIMDString simdstring11(simdstring10);

  EXPECT_EQ(simdstring0.size(), 0);
  EXPECT_STREQ(simdstring0.c_str(), "");

  EXPECT_EQ(simdstring1.size(), 1);
  EXPECT_STREQ(simdstring1.c_str(), "a");

  EXPECT_EQ(simdstring2.size(), 36);
  EXPECT_STREQ(simdstring2.c_str(), "0123456789abcdefghijklmnopqrstuvwxyz");

  EXPECT_EQ(simdstring3.size(), 26);
  EXPECT_STREQ(simdstring3.c_str(), "abcdefghijklmnopqrstuvwxyz");

  EXPECT_EQ(simdstring4.size(), 10);
  EXPECT_STREQ(simdstring4.c_str(), "abcdefghij");

  EXPECT_EQ(simdstring5.size(), 10);
  EXPECT_STREQ(simdstring5.c_str(), "0123456789");

  EXPECT_EQ(simdstring6.size(), 43);
  EXPECT_STREQ(simdstring6.c_str(), sampleString);

  EXPECT_EQ(simdstring7.size(), 3);
  EXPECT_STREQ(simdstring7.c_str(), "abc");

  EXPECT_EQ(simdstring8.size(), 3);
  EXPECT_STREQ(simdstring8.c_str(), "dog");

  EXPECT_EQ(simdstring9.size(), 5);
  EXPECT_STREQ(simdstring9.c_str(), "quick");

  EXPECT_EQ(simdstring10.size(), 10);
  EXPECT_STREQ(simdstring10.c_str(), "bbbbbbbbbb");

  simdstring10.insert(0, "0123456789");
  SIMDString simdstring12(simdstring10, 0, 5);

  EXPECT_EQ(simdstring11.size(), 10);
  EXPECT_STREQ(simdstring11.c_str(), "bbbbbbbbbb");

  EXPECT_EQ(simdstring12.size(), 5);
  EXPECT_STREQ(simdstring12.c_str(), "01234");
}

TEST(SIMDStringTest, Assign)
{
  SIMDString simdstring0;
  EXPECT_EQ(simdstring0.size(), 0);
  EXPECT_STREQ(simdstring0.c_str(), "");

  simdstring0.assign('a');
  EXPECT_EQ(simdstring0.size(), 1);
  EXPECT_STREQ(simdstring0.c_str(), "a");

  SIMDString simdstring1(sampleString, 26);

  SIMDString simdstring2("0123456789abcdefghijklmnopqrstuvwxyz");

  simdstring0.assign(simdstring2);
  EXPECT_EQ(simdstring0.size(), 36);
  EXPECT_STREQ(simdstring0.c_str(), "0123456789abcdefghijklmnopqrstuvwxyz");

  simdstring0.assign(simdstring1, 16, 5);
  EXPECT_EQ(simdstring0.size(), 5);
  EXPECT_STREQ(simdstring0.c_str(), "fox j");

  simdstring0.assign(simdstring2, 10);
  EXPECT_EQ(simdstring0.size(), 26);
  EXPECT_STREQ(simdstring0.c_str(), "abcdefghijklmnopqrstuvwxyz");

  simdstring0.assign(simdstring2, 10, 10);
  EXPECT_EQ(simdstring0.size(), 10);
  EXPECT_STREQ(simdstring0.c_str(), "abcdefghij");

  simdstring0.assign(simdstring2.begin(), simdstring2.begin() + 10);
  EXPECT_EQ(simdstring0.size(), 10);
  EXPECT_STREQ(simdstring0.c_str(), "0123456789");

  simdstring0 = SIMDString(sampleString);
  EXPECT_EQ(simdstring0.size(), 43);
  EXPECT_STREQ(simdstring0.c_str(), sampleString);

  simdstring0 = simdstring0;
  EXPECT_EQ(simdstring0.size(), 43);
  EXPECT_STREQ(simdstring0.c_str(), sampleString);

  simdstring0.assign({'a', 'b', 'c'});
  EXPECT_EQ(simdstring0.size(), 3);
  EXPECT_STREQ(simdstring0.c_str(), "abc");

  SIMDString simdstring3(100, 'a');
  std::string string3(100, 'a');
  simdstring0.assign(simdstring3);
  EXPECT_EQ(simdstring0.size(), string3.size());
  EXPECT_STREQ(simdstring0.c_str(), string3.c_str());

  simdstring0.assign(sampleString);
  EXPECT_EQ(simdstring0.size(), 43);
  EXPECT_STREQ(simdstring0.c_str(), sampleString);

  simdstring0.assign(simdstring2.c_str());
  EXPECT_EQ(simdstring0.size(), simdstring2.size());
  EXPECT_STREQ(simdstring0.c_str(), simdstring2.c_str());

  simdstring0.assign(string3.c_str());
  EXPECT_EQ(simdstring0.size(), string3.size());
  EXPECT_STREQ(simdstring0.c_str(), string3.c_str());

  // std::string string1(sampleString);
  // simdstring0.assign(string1, 34);
  // EXPECT_EQ(simdstring0.size(), 3);
  // EXPECT_STREQ(simdstring0.c_str(), "dog");

  // simdstring0.assign(string1, 4, 5);
  // EXPECT_EQ(simdstring0.size(), 5);
  // EXPECT_STREQ(simdstring0.c_str(), "brown");
}

TEST(SIMDStringTest, Access)
{
  SIMDString simdstring(sampleString);

  EXPECT_EQ(simdstring.at(4), 'q');
  EXPECT_EQ(simdstring[4], 'q');
  EXPECT_EQ(simdstring.front(), 't');
  EXPECT_EQ(simdstring.back(), 'g');
  EXPECT_EQ(simdstring.c_str(), sampleString);
  EXPECT_EQ(simdstring.data(), sampleString);
}

TEST(SIMDStringTest, Iterator)
{
  SIMDString simdstring(sampleString);

  EXPECT_EQ(*simdstring.begin(), 't');
  EXPECT_EQ(*(simdstring.end() - 1), 'g');
  EXPECT_EQ(*simdstring.cbegin(), 't');
  EXPECT_EQ(*(simdstring.cend() - 1), 'g');
  EXPECT_EQ(*simdstring.rbegin(), 'g');
  EXPECT_EQ(*(simdstring.rend() - 1), 't');
  EXPECT_EQ(*simdstring.crbegin(), 'g');
  EXPECT_EQ(*(simdstring.crend() - 1), 't');
}

TEST(SIMDStringTest, Compare)
{
  std::string string1(5, 'a');
  std::string string2(string1, 1);
  std::string string3(6, 'b');

  SIMDString simdstring1(5, 'a');
  SIMDString simdstring2(simdstring1, 1);
  SIMDString simdstring3(6, 'b');

  EXPECT_EQ(string1.compare(string1), simdstring1.compare(simdstring1));

  EXPECT_EQ(string1.compare(string2), simdstring1.compare(simdstring2));
  EXPECT_EQ(string2.compare(string1), simdstring2.compare(simdstring1));

  EXPECT_EQ(string1.compare(string3), simdstring1.compare(simdstring3));
  EXPECT_EQ(string3.compare(string1), simdstring3.compare(simdstring1));

  EXPECT_EQ(string3.compare(string2), simdstring3.compare(simdstring2));
  EXPECT_EQ(string2.compare(string3), simdstring2.compare(simdstring3));

  EXPECT_EQ(string1.compare(simdstring1.c_str()), simdstring1.compare(string1.c_str()));

  EXPECT_EQ(string1.compare(simdstring2.c_str()), simdstring1.compare(string2.c_str()));
  EXPECT_EQ(string2.compare(simdstring1.c_str()), simdstring2.compare(string1.c_str()));

  EXPECT_EQ(string1.compare(simdstring3.c_str()), simdstring1.compare(string3.c_str()));
  EXPECT_EQ(string3.compare(simdstring1.c_str()), simdstring3.compare(string1.c_str()));

  EXPECT_EQ(string3.compare(simdstring2.c_str()), simdstring3.compare(string2.c_str()));
  EXPECT_EQ(string2.compare(simdstring3.c_str()), simdstring2.compare(string3.c_str()));

  EXPECT_EQ(string1 < string2, simdstring1 < simdstring2);
  EXPECT_EQ(string2 < string1, simdstring2 < simdstring1);
  EXPECT_EQ(string1 < string3, simdstring1 < simdstring3);
  EXPECT_EQ(string3 < string1, simdstring3 < simdstring1);
  EXPECT_EQ(string3 < string2, simdstring3 < simdstring2);
  EXPECT_EQ(string2 < string3, simdstring2 < simdstring3);

  EXPECT_EQ(string1 <= string2, simdstring1 <= simdstring2);
  EXPECT_EQ(string2 <= string1, simdstring2 <= simdstring1);
  EXPECT_EQ(string1 <= string3, simdstring1 <= simdstring3);
  EXPECT_EQ(string3 <= string1, simdstring3 <= simdstring1);
  EXPECT_EQ(string3 <= string2, simdstring3 <= simdstring2);
  EXPECT_EQ(string2 <= string3, simdstring2 <= simdstring3);

  EXPECT_EQ(string1 > string2, simdstring1 > simdstring2);
  EXPECT_EQ(string2 > string1, simdstring2 > simdstring1);
  EXPECT_EQ(string1 > string3, simdstring1 > simdstring3);
  EXPECT_EQ(string3 > string1, simdstring3 > simdstring1);
  EXPECT_EQ(string3 > string2, simdstring3 > simdstring2);
  EXPECT_EQ(string2 > string3, simdstring2 > simdstring3);

  EXPECT_EQ(string1 >= string2, simdstring1 >= simdstring2);
  EXPECT_EQ(string2 >= string1, simdstring2 >= simdstring1);
  EXPECT_EQ(string1 >= string3, simdstring1 >= simdstring3);
  EXPECT_EQ(string3 >= string1, simdstring3 >= simdstring1);
  EXPECT_EQ(string3 >= string2, simdstring3 >= simdstring2);
  EXPECT_EQ(string2 >= string3, simdstring2 >= simdstring3);
}

TEST(SIMDStringTest, Equality)
{
  SIMDString simdstring1(5, 'a');
  SIMDString simdstring2(6, 'a');
  SIMDString simdstring3(6, 'b');
  std::string string1(5, 'a');
  std::string string2(6, 'a');
  std::string string3(6, 'b');

  EXPECT_TRUE(simdstring1 == simdstring1);
  EXPECT_TRUE(simdstring2 == simdstring2);
  EXPECT_TRUE(simdstring3 == simdstring3);

  EXPECT_FALSE(simdstring1 == simdstring2);
  EXPECT_FALSE(simdstring2 == simdstring1);

  EXPECT_FALSE(simdstring3 == simdstring2);
  EXPECT_FALSE(simdstring2 == simdstring3);

  EXPECT_FALSE(simdstring1 == simdstring3);
  EXPECT_FALSE(simdstring3 == simdstring1);

  EXPECT_TRUE(simdstring1 == string1.c_str());
  EXPECT_TRUE(simdstring2 == string2.c_str());
  EXPECT_TRUE(simdstring3 == string3.c_str());

  EXPECT_FALSE(simdstring1 == string2.c_str());
  EXPECT_FALSE(simdstring2 == string1.c_str());

  EXPECT_FALSE(simdstring3 == string2.c_str());
  EXPECT_FALSE(simdstring2 == string3.c_str());

  EXPECT_FALSE(simdstring1 == string3.c_str());
  EXPECT_FALSE(simdstring3 == string1.c_str());

  EXPECT_FALSE(simdstring3 == "");
}

TEST(SIMDStringTest, Append)
{
  std::string string1(5, 'a');
  std::string string2(6, 'b');

  SIMDString simdstring1(5, 'a');
  SIMDString simdstring2(6, 'b');

  EXPECT_STREQ((string1 + "abc").c_str(), (simdstring1 + "abc").c_str());
  EXPECT_STREQ((string1 + 'a').c_str(), (simdstring1 + 'a').c_str());

  // append calls operator+=
  EXPECT_STREQ(string1.append(string2).c_str(), simdstring1.append(simdstring2).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  EXPECT_STREQ(string1.append(string2, 2).c_str(), simdstring1.append(simdstring2, 2).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  EXPECT_STREQ(string1.append(string2, 2, 2).c_str(), simdstring1.append(simdstring2, 2, 2).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  EXPECT_STREQ(string1.append("0123456789").c_str(), simdstring1.append("0123456789").c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  EXPECT_STREQ(string1.append(sampleString).c_str(), simdstring1.append(sampleString).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  EXPECT_STREQ(string1.append(6, ' ').c_str(), simdstring1.append(6, ' ').c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  EXPECT_STREQ(string1.append({'d', '1', 'h'}).c_str(), simdstring1.append({'d', '1', 'h'}).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  EXPECT_STREQ((string1 + string2).c_str(), (simdstring1 + simdstring2).c_str());
  EXPECT_STREQ((string1 + sampleString).c_str(), (simdstring1 + sampleString).c_str());
  EXPECT_STREQ((string1 + 'a').c_str(), (simdstring1 + 'a').c_str());
}

TEST(SIMDStringTest, PushPopBack)
{
  SIMDString simdstring1(sampleString);
  std::string string1(sampleString);

  string1.push_back('a');
  simdstring1.push_back('a');

  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  string1.pop_back();
  simdstring1.pop_back();
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  SIMDString simdstring2;
  simdstring2.push_back('a');
  EXPECT_STREQ(simdstring2.c_str(), "a");
  EXPECT_EQ(simdstring2.length(), 1);
}

TEST(SIMDStringTest, Insert)
{

  std::string string1("0123456789");
  std::string string2("abcdefg");

  SIMDString simdstring1("0123456789");
  SIMDString simdstring2("abcdefg");

  EXPECT_STREQ(string1.insert(0, string2).c_str(), simdstring1.insert(0, simdstring2).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.insert(0, string2, 2, 3).c_str(), simdstring1.insert(0, simdstring2, 2, 3).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.insert(0, "hijklmnop").c_str(), simdstring1.insert(0, "hijklmnop").c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.insert(0, "hijklmnop", 4).c_str(), simdstring1.insert(0, "hijklmnop", 4).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.insert(0, 6, 'z').c_str(), simdstring1.insert(0, 6, 'z').c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  EXPECT_STREQ(string1.insert(5, string2).c_str(), simdstring1.insert(5, simdstring2).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.insert(5, string2, 2, 3).c_str(), simdstring1.insert(5, simdstring2, 2, 3).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.insert(5, "hijklmnop").c_str(), simdstring1.insert(5, "hijklmnop").c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.insert(5, "hijklmnop", 4).c_str(), simdstring1.insert(5, "hijklmnop", 4).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.insert(5, 6, 'z').c_str(), simdstring1.insert(5, 6, 'z').c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  EXPECT_STREQ(string1.insert(10, string2).c_str(), simdstring1.insert(10, simdstring2).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.insert(10, string2, 2, 3).c_str(), simdstring1.insert(10, simdstring2, 2, 3).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.insert(10, "hijklmnop").c_str(), simdstring1.insert(10, "hijklmnop").c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.insert(10, "hijklmnop", 4).c_str(), simdstring1.insert(10, "hijklmnop", 4).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.insert(10, 6, 'z').c_str(), simdstring1.insert(10, 6, 'z').c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
}

TEST(SIMDStringTest, Replace)
{

  std::string string1("0123456789");
  std::string string2("abcdefg");

  SIMDString simdstring1("0123456789");
  SIMDString simdstring2("abcdefg");

  EXPECT_STREQ(string1.replace(0, 1, string2).c_str(), simdstring1.replace(0, 1, simdstring2).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(0, 1, string2, 2, 3).c_str(), simdstring1.replace(0, 1, simdstring2, 2, 3).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(0, 1, "hijklmnop").c_str(), simdstring1.replace(0, 1, "hijklmnop").c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(0, 1, "hijklmnop", 4).c_str(), simdstring1.replace(0, 1, "hijklmnop", 4).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(0, 1, 6, 'z').c_str(), simdstring1.replace(0, 1, 6, 'z').c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(0, 5, 2, 'd').c_str(), simdstring1.replace(0, 5, 2, 'd').c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  EXPECT_STREQ(string1.replace(5, 5, string2).c_str(), simdstring1.replace(5, 5, simdstring2).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(5, 5, string2, 2, 3).c_str(), simdstring1.replace(5, 5, simdstring2, 2, 3).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(5, 5, "hijklmnop").c_str(), simdstring1.replace(5, 5, "hijklmnop").c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(5, 5, "hijklmnop", 4).c_str(), simdstring1.replace(5, 5, "hijklmnop", 4).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(5, 5, 6, 'z').c_str(), simdstring1.replace(5, 5, 6, 'z').c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(5, 5, 2, 'd').c_str(), simdstring1.replace(5, 5, 2, 'd').c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  EXPECT_STREQ(string1.replace(10, 0, string2).c_str(), simdstring1.replace(10, 0, simdstring2).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(10, 0, string2, 2, 3).c_str(), simdstring1.replace(10, 0, simdstring2, 2, 3).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(10, 0, "hijklmnop").c_str(), simdstring1.replace(10, 0, "hijklmnop").c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(10, 0, "hijklmnop", 4).c_str(), simdstring1.replace(10, 0, "hijklmnop", 4).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(10, 0, 6, 'z').c_str(), simdstring1.replace(10, 0, 6, 'z').c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(10, 5, 2, 'd').c_str(), simdstring1.replace(10, 5, 2, 'd').c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());


  EXPECT_STREQ(string1.replace(5, 5, "hijklmnop", 5).c_str(), simdstring1.replace(5, 5, "hijklmnop", 5).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
  EXPECT_STREQ(string1.replace(5, 5, 5, 'z').c_str(), simdstring1.replace(5, 5, 5, 'z').c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
}

TEST(SIMDStringTest, ClearErase)
{
  SIMDString simdstring1(sampleString);
  std::string string1(sampleString);

  EXPECT_EQ(simdstring1.erase(6, 12).size(), string1.erase(6, 12).size());
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  string1.erase(string1.begin() + 5);
  simdstring1.erase(simdstring1.begin() + 5);
  EXPECT_EQ(simdstring1.size(), string1.size());
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_FALSE(simdstring1.empty());

  string1.erase(string1.begin(), string1.begin() + 5);
  simdstring1.erase(simdstring1.begin(), simdstring1.begin() + 5);
  EXPECT_EQ(simdstring1.size(), string1.size());
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_FALSE(simdstring1.empty());

  simdstring1.erase(simdstring1.begin(), simdstring1.end());
  EXPECT_EQ(simdstring1.size(), 0);
  EXPECT_EQ(simdstring1.length(), 0);
  EXPECT_STREQ(simdstring1.c_str(), "");
  EXPECT_TRUE(simdstring1.empty());


  SIMDString simdstring2(sampleString);
  simdstring2.clear();
  EXPECT_EQ(simdstring2.size(), 0);
  EXPECT_EQ(simdstring2.length(), 0);
  EXPECT_STREQ(simdstring2.c_str(), "");
  EXPECT_TRUE(simdstring2.empty());
}

TEST(SIMDStringTest, Swap)
{
  std::string string1("abcabcabcabcabcabcabcdabcabcabcabc");
  SIMDString simdstring1("abcabcabcabcabcabcabcdabcabcabcabc");

  std::string string2("aaaaaaaaaaaaaaaaabaaaaaaaa");
  SIMDString simdstring2("aaaaaaaaaaaaaaaaabaaaaaaaa");

  std::string string3(100, 'a');
  SIMDString simdstring3(100, 'a');

  simdstring1.swap(simdstring2);

  EXPECT_STREQ(simdstring1.c_str(), string2.c_str());
  EXPECT_EQ(simdstring1.size(), string2.size());
  EXPECT_STREQ(simdstring2.c_str(), string1.c_str());
  EXPECT_EQ(simdstring2.size(), string1.size());

  // simdstring1 = string2
  simdstring1.swap(simdstring3);
  EXPECT_STREQ(simdstring1.c_str(), string3.c_str());
  EXPECT_EQ(simdstring1.size(), string3.size());
  EXPECT_STREQ(simdstring3.c_str(), string2.c_str());
  EXPECT_EQ(simdstring3.size(), string2.size());
}

TEST(SIMDStringTest, Find)
{
  std::string string1("abcabcabcabcabcabcabcdabcabcabcabc");
  SIMDString simdstring1("abcabcabcabcabcabcabcdabcabcabcabc");

  std::string string2("aaaaaaaaaaaaaaaaabaaaaaaaa");
  SIMDString simdstring2("aaaaaaaaaaaaaaaaabaaaaaaaa");

  std::string string3(10, '-');
  SIMDString simdstring3(10, '-');
  std::string string4(8, '*');
  SIMDString simdstring4(8, '*');

  EXPECT_EQ(string1.find("abcd"), simdstring1.find("abcd"));
  EXPECT_EQ(string1.find("abcabcabcabcabcabcabcdabcabcabcabcasdf"), simdstring1.find("abcabcabcabcabcabcabcdabcabcabcabcasdf"));
  EXPECT_EQ(string1.find('d'), simdstring1.find('d'));
  EXPECT_EQ(string2.find("ab"), simdstring2.find("ab"));
  EXPECT_EQ(string3.find(string4), simdstring3.find(simdstring4));

  SIMDString simdstring5; 
  std::string string5; 

  EXPECT_EQ(string5.find("abcd"), simdstring5.find("abcd"));
}

TEST(SIMDStringTest, RFind)
{
  std::string string1("abcabcabcabcabcabcabcdabcabcabcabc");
  SIMDString simdstring1("abcabcabcabcabcabcabcdabcabcabcabc");

  std::string string2("aaaaaaaaaaaaaaaaabaaaaaaaa");
  SIMDString simdstring2("aaaaaaaaaaaaaaaaabaaaaaaaa");

  std::string string3(10, '-');
  SIMDString simdstring3(10, '-');
  std::string string4(8, '*');
  SIMDString simdstring4(8, '*');

  EXPECT_EQ(string1.rfind("abcd"), simdstring1.rfind("abcd"));
  EXPECT_EQ(string1.rfind("abcd", 10), simdstring1.rfind("abcd", 10));
  EXPECT_EQ(string1.rfind("abcabcabcabcabcabcabcdabcabcabcabcasdf"), simdstring1.rfind("abcabcabcabcabcabcabcdabcabcabcabcasdf"));
  EXPECT_EQ(string1.rfind('d'), simdstring1.rfind('d'));
  EXPECT_EQ(string1.rfind('e'), simdstring1.rfind('e'));
  EXPECT_EQ(string2.rfind("ab"), simdstring2.rfind("ab"));
  EXPECT_EQ(string3.rfind(string4), simdstring3.rfind(simdstring4));
}

TEST(SIMDStringTest, FindFirstLastOf)
{
  std::string string1("The quick brown fox jumps over the lazy dog. Sphinx of black quartz, judge my vow.");
  SIMDString simdstring1("The quick brown fox jumps over the lazy dog. Sphinx of black quartz, judge my vow.");

  EXPECT_EQ(string1.find_first_of('d'), simdstring1.find_first_of('d'));
  EXPECT_EQ(string1.find_first_of("mnop"), simdstring1.find_first_of("mnop"));

  EXPECT_EQ(string1.find_last_of('d'), simdstring1.find_last_of('d'));
  EXPECT_EQ(string1.find_last_of("mnop"), simdstring1.find_last_of("mnop"));

  EXPECT_EQ(string1.find_first_of('d', 40), simdstring1.find_first_of('d', 40));
  EXPECT_EQ(string1.find_first_of("mnop", 40), simdstring1.find_first_of("mnop", 40));

  EXPECT_EQ(string1.find_last_of('d', 40), simdstring1.find_last_of('d', 40));
  EXPECT_EQ(string1.find_last_of("mnop", 40), simdstring1.find_last_of("mnop", 40));

  EXPECT_EQ(string1.find_first_of('T'), simdstring1.find_first_of('T'));
  EXPECT_EQ(string1.find_first_of("RST"), simdstring1.find_first_of("RST"));

  EXPECT_EQ(string1.find_last_of('.'), simdstring1.find_last_of('.'));
  EXPECT_EQ(string1.find_last_of(".,!?"), simdstring1.find_last_of(".,!?"));

  EXPECT_EQ(string1.find_first_of('q', 4), simdstring1.find_first_of('q', 4));
  EXPECT_EQ(string1.find_first_of("mnop", 14), simdstring1.find_first_of("mnop", 14));

  EXPECT_EQ(string1.find_last_of('d', 71), simdstring1.find_last_of('d', 71));
  EXPECT_EQ(string1.find_last_of("jkl", 69), simdstring1.find_last_of("jkl", 69));

  EXPECT_EQ(string1.find_first_of('?'), simdstring1.find_first_of('?'));
  EXPECT_EQ(string1.find_first_of("1234"), simdstring1.find_first_of("1234"));
  EXPECT_EQ(string1.find_last_of('?'), simdstring1.find_last_of('?'));
  EXPECT_EQ(string1.find_last_of("1234"), simdstring1.find_last_of("1234"));


  EXPECT_EQ(string1.find_first_of('?', 90), simdstring1.find_first_of('?', 90));
  EXPECT_EQ(string1.find_first_of("1234", 90), simdstring1.find_first_of("1234", 90));
  EXPECT_EQ(string1.find_last_of('?', 90), simdstring1.find_last_of('?', 90));
  EXPECT_EQ(string1.find_last_of("1234", 90), simdstring1.find_last_of("1234", 90));
}

TEST(SIMDStringTest, FindFirstLastNotOf)
{
  std::string string1("     a           b     m       c            d            e    t     f         ");
  SIMDString simdstring1("     a           b     m       c            d            e    t     f         ");

  EXPECT_EQ(string1.find_first_not_of(' '), simdstring1.find_first_not_of(' '));
  // the space in "abcdef " is important
  EXPECT_EQ(string1.find_first_not_of("abcdef "), simdstring1.find_first_not_of("abcdef "));

  EXPECT_EQ(string1.find_first_not_of(' ', 35), simdstring1.find_first_not_of(' ', 35));
  EXPECT_EQ(string1.find_first_not_of("abcdef ", 35), simdstring1.find_first_not_of("abcdef ", 35));

  EXPECT_EQ(string1.find_last_not_of(' '), simdstring1.find_last_not_of(' '));
  EXPECT_EQ(string1.find_last_not_of("abcdef "), simdstring1.find_last_not_of("abcdef "));

  EXPECT_EQ(string1.find_last_not_of(' ', 35), simdstring1.find_last_not_of(' ', 35));
  EXPECT_EQ(string1.find_last_not_of("abcdef ", 35), simdstring1.find_last_not_of("abcdef ", 35));

  // no space in "abcdef", we're searching for spaces here
  EXPECT_EQ(string1.find_first_not_of('a'), simdstring1.find_first_not_of('a'));
  EXPECT_EQ(string1.find_first_not_of("abcdef"), simdstring1.find_first_not_of("abcdef"));

  EXPECT_EQ(string1.find_first_not_of('a', 35), simdstring1.find_first_not_of('a', 35));
  EXPECT_EQ(string1.find_first_not_of("abcdef", 35), simdstring1.find_first_not_of("abcdef", 35));

  EXPECT_EQ(string1.find_last_not_of('a'), simdstring1.find_last_not_of('a'));
  EXPECT_EQ(string1.find_last_not_of("abcdef"), simdstring1.find_last_not_of("abcdef"));

  EXPECT_EQ(string1.find_last_not_of('a', 35), simdstring1.find_last_not_of('a', 35));
  EXPECT_EQ(string1.find_last_not_of("abcdef", 35), simdstring1.find_last_not_of("abcdef", 35));

  SIMDString simdstring2 ("aaaaaaaa");
  std::string string2("aaaaaaaa");

  EXPECT_EQ(string2.find_first_not_of('a'), simdstring2.find_first_not_of('a'));
  EXPECT_EQ(string2.find_first_not_of("abcdef"), simdstring2.find_first_not_of("abcdef"));
  EXPECT_EQ(string2.find_last_not_of('a'), simdstring2.find_last_not_of('a'));
  EXPECT_EQ(string2.find_last_not_of("abcdef"), simdstring2.find_last_not_of("abcdef"));

  EXPECT_EQ(string2.find_first_not_of('?', 90), simdstring2.find_first_not_of('?', 90));
  EXPECT_EQ(string2.find_first_not_of("1234", 90), simdstring2.find_first_not_of("1234", 90));
  EXPECT_EQ(string2.find_last_not_of('?', 90), simdstring2.find_last_not_of('?', 90));
  EXPECT_EQ(string2.find_last_not_of("1234", 90), simdstring2.find_last_not_of("1234", 90));

}

TEST(SIMDStringTest, StartsEndsWith)
{
  SIMDString simdstring1(sampleString);
  std::string string1(sampleString);

  EXPECT_TRUE(simdstring1.starts_with("the"));
  EXPECT_FALSE(simdstring1.starts_with("woah"));
  EXPECT_TRUE(simdstring1.ends_with("dog"));
  EXPECT_FALSE(simdstring1.ends_with("woah"));
}

TEST(SIMDStringTest, CopySubstr)
{

  char tmp[11];
  SIMDString simdstring1("0123456789abcdefghijklmnopqrstuvwxyz");
  tmp[10] = 0;

  EXPECT_EQ(10, simdstring1.copy(tmp, 10));
  EXPECT_STREQ(tmp, "0123456789");
  EXPECT_STREQ(tmp, simdstring1.substr(0, 10).c_str());

  EXPECT_EQ(10, simdstring1.copy(tmp, 10, 10));
  EXPECT_STREQ(tmp, "abcdefghij");
  EXPECT_STREQ(tmp, simdstring1.substr(10, 10).c_str());

  EXPECT_STREQ("abcdefghijklmnopqrstuvwxyz", simdstring1.substr(10).c_str());
}

TEST(SIMDStringTest, Resize)
{
  SIMDString simdstring1(sampleString);
  std::string string1(sampleString);

  simdstring1.resize(50);
  string1.resize(50);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1.resize(50, '!');
  string1.resize(50, '!');
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1.resize(40);
  string1.resize(40);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1.resize(40, '!');
  string1.resize(40, '!');
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());
}

TEST(SIMDStringTest, Size)
{
  SIMDString simdstring1(sampleString);
  std::string string1(sampleString);

  EXPECT_EQ(simdstring1.size(), string1.size());
  EXPECT_EQ(simdstring1.length(), string1.length());

  EXPECT_FALSE(simdstring1.empty());
  simdstring1.clear();
  EXPECT_TRUE(simdstring1.empty());
}

TEST(SIMDStringTest, Conversions)
{
  SIMDString simdstring1 = to_string(1234567890);
  std::string string1 = std::to_string(1234567890);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  simdstring1 = to_string(1234567890u);
  string1 = std::to_string(1234567890u);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  simdstring1 = to_string(1234567890l);
  string1 = std::to_string(1234567890l);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  simdstring1 = to_string(1234567890ul);
  string1 = std::to_string(1234567890ul);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  simdstring1 = to_string(1234567890ll);
  string1 = std::to_string(1234567890ll);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  simdstring1 = to_string(1234567890ull);
  string1 = std::to_string(1234567890ull);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());


  simdstring1 = to_string(0X1.BC70A3D70A3D7P+6f);
  string1 = std::to_string(0X1.BC70A3D70A3D7P+6f);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());


  simdstring1 = to_string(0X1.BC70A3D70A3D7P+6);
  string1 = std::to_string(0X1.BC70A3D70A3D7P+6);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());


  simdstring1 = to_string(0X1.BC70A3D70A3D7P+6l);
  string1 = std::to_string(0X1.BC70A3D70A3D7P+6l);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  SIMDString simdstring2("-1234567890");
  std::string string2("-1234567890");

  EXPECT_EQ(stoi(simdstring2), std::stoi(string2));
  EXPECT_EQ(stol(simdstring2), std::stol(string2));
  EXPECT_EQ(stoll(simdstring2), std::stoll(string2));

  EXPECT_EQ(stoul(simdstring2), std::stol(string2));
  EXPECT_EQ(stoull(simdstring2), std::stoll(string2));

  size_t posSimdstring, posString;
  EXPECT_EQ(stoi(simdstring2, &posSimdstring), std::stoi(string2, &posString));
  EXPECT_EQ(posSimdstring, posString);
  EXPECT_EQ(stol(simdstring2, &posSimdstring), std::stol(string2, &posString));
  EXPECT_EQ(posSimdstring, posString);
  EXPECT_EQ(stoll(simdstring2, &posSimdstring), std::stoll(string2, &posString));
  EXPECT_EQ(posSimdstring, posString);

  EXPECT_EQ(stoul(simdstring2, &posSimdstring), std::stol(string2, &posString));
  EXPECT_EQ(posSimdstring, posString);
  EXPECT_EQ(stoull(simdstring2, &posSimdstring), std::stoll(string2, &posString));
  EXPECT_EQ(posSimdstring, posString);

  SIMDString simdstring3("-1.52343249");
  std::string string3("-1.52343249");

  EXPECT_EQ(stof(simdstring3), std::stof(string3));
  EXPECT_EQ(stod(simdstring3), std::stod(string3));
  EXPECT_EQ(stold(simdstring3), std::stold(string3));

  EXPECT_EQ(stof(simdstring3, &posSimdstring), std::stof(string3, &posString));
  EXPECT_EQ(posSimdstring, posString);
  EXPECT_EQ(stod(simdstring3, &posSimdstring), std::stod(string3, &posString));
  EXPECT_EQ(posSimdstring, posString);
  EXPECT_EQ(stold(simdstring3, &posSimdstring), std::stold(string3, &posString));
  EXPECT_EQ(posSimdstring, posString);
}

TEST(SIMDStringTest, IO){
  std::istringstream iss1("Hello there!\nWho are you?");
  std::istringstream iss2("Hello there!\nWho are you?");

  std::string tmp(1 << 21, '-');
  std::istringstream iss3(tmp);
  std::istringstream iss4(tmp);
  std::istringstream iss5(tmp);
  std::istringstream iss6(tmp);

  SIMDString simdstring1;
  std::string string1;

  iss1 >> simdstring1;
  iss2 >> string1;
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  std::getline(iss1, string1);
  getline(iss2, simdstring1);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  iss3 >> simdstring1;
  iss4 >> string1;
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  std::getline(iss5, string1);
  getline(iss6, simdstring1);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  std::ostringstream oss2;
  std::ostringstream oss1;

  oss1 << simdstring1; 
  oss2 << string1; 
  EXPECT_STREQ(oss1.str().c_str(), oss2.str().c_str());
}


TEST(SIMDStringTest, CompileError){
  SIMDString<32> simdstring1;
}