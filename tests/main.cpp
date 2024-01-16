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
size_t sampleStringSize = strlen(sampleString);
char sampleStringLarge [446] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
size_t sampleStringLargeSize = strlen(sampleStringLarge);
const char* findTestString = "abcabcabcabcabcabcabcdabcabcABCabc";
const char* findTestString2 = "abcabcabcabcabcabcabcdabcabcabcabcadsf";
const char* findTestString3 = "abcabcabcabcabcabcabcabcabcABCabc";

TEST(SIMDStringTest, Construct)
{
  SIMDString<64> simdstring0;
  SIMDString<64> simdstring1('a');
  SIMDString<64> simdstring2("0123456789abcdefghijklmnopqrstuvwxyz");
  SIMDString<64> simdstring3(simdstring2, 10);
  SIMDString<64> simdstring4(simdstring2, 10, 10);
  SIMDString<64> simdstring5(simdstring2.begin(), simdstring2.begin() + 10);
  SIMDString<64> simdstring6{SIMDString<64>(sampleString)};
  SIMDString<64> simdstring7({'a', 'b', 'c'});

  std::string string1(sampleString);
  SIMDString<64> simdstring8(string1, 40);
  SIMDString<64> simdstring9(string1, 4, 5);
  SIMDString<64> simdstring10(10, 'b');
  SIMDString<64> simdstring11(simdstring10);

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
  SIMDString<64> simdstring12(simdstring10, 0, 5);

  EXPECT_EQ(simdstring11.size(), 10);
  EXPECT_STREQ(simdstring11.c_str(), "bbbbbbbbbb");

  EXPECT_EQ(simdstring12.size(), 5);
  EXPECT_STREQ(simdstring12.c_str(), "01234");

  SIMDString<64> simdstring13 (string1.begin(), string1.end());
  EXPECT_EQ(simdstring13.size(), string1.size());
  EXPECT_STREQ(simdstring13.c_str(), string1.c_str());
  
  std::istringstream ss{sampleStringLarge};
  std::istreambuf_iterator<char> it{ss};
  SIMDString<64> simdstring14{it, std::istreambuf_iterator<char>()};
  EXPECT_STREQ(simdstring14.c_str(), sampleStringLarge);
  EXPECT_EQ(simdstring14.size(), sampleStringLargeSize);
}

TEST(SIMDStringTest, Assign)
{
  SIMDString<64> simdstring0;
  EXPECT_EQ(simdstring0.size(), 0);
  EXPECT_STREQ(simdstring0.c_str(), "");

  simdstring0.assign(1, 'a');
  EXPECT_EQ(simdstring0.size(), 1);
  EXPECT_STREQ(simdstring0.c_str(), "a");

  SIMDString<64> simdstring1(sampleString, 26);

  SIMDString<64> simdstring2("0123456789abcdefghijklmnopqrstuvwxyz");

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

  simdstring0 = SIMDString<64>(sampleString);
  EXPECT_EQ(simdstring0.size(), 43);
  EXPECT_STREQ(simdstring0.c_str(), sampleString);

  simdstring0 = simdstring0;
  EXPECT_EQ(simdstring0.size(), 43);
  EXPECT_STREQ(simdstring0.c_str(), sampleString);

  simdstring0.assign({'a', 'b', 'c'});
  EXPECT_EQ(simdstring0.size(), 3);
  EXPECT_STREQ(simdstring0.c_str(), "abc");

  SIMDString<64> simdstring3(100, 'a');
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

  std::string string4(40, 'c');
  simdstring0 = string4;
  EXPECT_EQ(simdstring0.size(), string4.size());
  EXPECT_STREQ(simdstring0.c_str(), string4.c_str());

  simdstring0 = "";
  EXPECT_EQ(simdstring0.size(), 0);
  EXPECT_STREQ(simdstring0.c_str(), "");
}

TEST(SIMDStringTest, Access)
{
  SIMDString<64> simdstring(sampleString);

  EXPECT_EQ(simdstring.at(4), 'q');
  EXPECT_EQ(simdstring[4], 'q');
  EXPECT_EQ(simdstring.front(), 't');
  EXPECT_EQ(simdstring.back(), 'g');
  EXPECT_STREQ(simdstring.c_str(), sampleString);
  EXPECT_STREQ(simdstring.data(), sampleString);

  const char* aString = "aaaaa";
  const char* constString1 = "baaaa";
  const char* constString2 = "abaaa";
  const char* constString3 = "aabaa";
  const char* constString4 = "aaaab";

  SIMDString<64> simdstring1(constString1);
  EXPECT_EQ(simdstring1.c_str(), constString1);
  simdstring1.front() = 'a';
  EXPECT_NE(simdstring1.c_str(), constString1);
  EXPECT_STREQ(simdstring1.c_str(), aString);

  SIMDString<64> simdstring2(constString2);
  EXPECT_EQ(simdstring2.c_str(), constString2);
  simdstring2[1] = 'a';
  EXPECT_NE(simdstring2.c_str(), constString2);
  EXPECT_STREQ(simdstring2.c_str(), aString);

  SIMDString<64> simdstring3(constString3);
  EXPECT_EQ(simdstring3.c_str(), constString3);
  simdstring3.at(2) = 'a';
  EXPECT_NE(simdstring3.c_str(), constString3);
  EXPECT_STREQ(simdstring3.c_str(), aString);

  SIMDString<64> simdstring4(constString4);
  EXPECT_EQ(simdstring4.c_str(), constString4);
  simdstring4.back() = 'a';
  EXPECT_NE(simdstring4.c_str(), constString4);
  EXPECT_STREQ(simdstring4.c_str(), aString);
}

TEST(SIMDStringTest, Iterator)
{
  SIMDString<64> simdstring1(sampleString);
  std::string string1(sampleString);

  EXPECT_EQ(*simdstring1.begin(), *string1.begin());
  EXPECT_EQ(*(simdstring1.end() - 1), *(string1.end() - 1)); 
  EXPECT_EQ(*simdstring1.cbegin(), *string1.cbegin());
  EXPECT_EQ(*(simdstring1.cend() - 1), *(string1.cend() - 1));
  EXPECT_EQ(*simdstring1.rbegin(), *string1.rbegin());
  EXPECT_EQ(*(simdstring1.rend() - 1), *(string1.rend() - 1));
  EXPECT_EQ(*simdstring1.crbegin(), *string1.crbegin());
  EXPECT_EQ(*(simdstring1.crend() - 1), *(string1.crend() - 1));

  // test mutability
  *simdstring1.begin() = 'a';
  *string1.begin() = 'a';
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());

  *(simdstring1.end() - 16) = '1';
  *(string1.end() - 16) = '1';
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());

  *(simdstring1.rbegin() + 10) = '5';
  *(string1.rbegin() + 10) = '5';
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());

  *(simdstring1.rend() - 30) = 'T';
  *(string1.rend() - 30) = 'T';
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
}

TEST(SIMDStringTest, Compare)
{
  std::string string1(5, 'a');
  std::string string2(string1, 1);
  std::string string3(6, 'b');
  std::string string4(5, 'a');

  SIMDString<64> simdstring1(5, 'a');
  SIMDString<64> simdstring2(simdstring1, 1);
  SIMDString<64> simdstring3(6, 'b');
  SIMDString<64> simdstring4(5, 'a');

  // same string
  EXPECT_EQ(string1.compare(string1), simdstring1.compare(simdstring1));
  EXPECT_EQ(string1.compare(simdstring1.c_str()), simdstring1.compare(string1.c_str()));
  EXPECT_EQ(string1.compare(string4), simdstring1.compare(simdstring4));
  EXPECT_EQ(string1.compare(simdstring4.c_str()), simdstring1.compare(string4.c_str()));

  // one off
  EXPECT_EQ(string1.compare(string2), simdstring1.compare(simdstring2));
  EXPECT_EQ(string2.compare(string1), simdstring2.compare(simdstring1));
  EXPECT_EQ(string1.compare(simdstring2.c_str()), simdstring1.compare(string2.c_str()));
  EXPECT_EQ(string2.compare(simdstring1.c_str()), simdstring2.compare(string1.c_str()));

  // totally different
  EXPECT_EQ(string1.compare(string3), simdstring1.compare(simdstring3));
  EXPECT_EQ(string3.compare(string1), simdstring3.compare(simdstring1));
  EXPECT_EQ(string1.compare(simdstring3.c_str()), simdstring1.compare(string3.c_str()));
  EXPECT_EQ(string3.compare(simdstring1.c_str()), simdstring3.compare(string1.c_str()));
  EXPECT_EQ(string3.compare(string2), simdstring3.compare(simdstring2));
  EXPECT_EQ(string2.compare(string3), simdstring2.compare(simdstring3));
  EXPECT_EQ(string3.compare(simdstring2.c_str()), simdstring3.compare(string2.c_str()));
  EXPECT_EQ(string2.compare(simdstring3.c_str()), simdstring2.compare(string3.c_str()));

  // compare with pos
  EXPECT_EQ(string1.compare(1, 2, string3, 1, 2), simdstring1.compare(1, 2, simdstring3, 1, 2));
  EXPECT_EQ(string3.compare(1, 2, string1, 1, 2), simdstring3.compare(1, 2, simdstring1, 1, 2));
  EXPECT_EQ(string1.compare(1, 2, simdstring3.c_str(), 2), simdstring1.compare(1, 2, string3.c_str(), 2));
  EXPECT_EQ(string3.compare(1, 2, simdstring1.c_str(), 2), simdstring3.compare(1, 2, string1.c_str(), 2));
  
  // operators
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
  SIMDString<64> simdstring1(5, 'a');
  SIMDString<64> simdstring2(6, 'a');
  SIMDString<64> simdstring3(6, 'b');
  SIMDString<64> simdstring4(5, 'a');
  std::string string1(5, 'a');
  std::string string2(6, 'a');
  std::string string3(6, 'b');

  EXPECT_TRUE(simdstring1 == simdstring1);
  EXPECT_TRUE(simdstring2 == simdstring2);
  EXPECT_TRUE(simdstring3 == simdstring3);
  EXPECT_TRUE(simdstring1 == simdstring4);

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

  SIMDString<64> simdstring1(5, 'a');
  SIMDString<64> simdstring2(6, 'b');

  EXPECT_STREQ((string1 + "abc").c_str(), (simdstring1 + "abc").c_str());
  EXPECT_STREQ((string1 + 'a').c_str(), (simdstring1 + 'a').c_str());

  // append calls operator+=, so this tests that as well
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

  EXPECT_STREQ(string1.append(string2.begin() + 1, string2.begin() + 3).c_str(), simdstring1.append(simdstring2.begin() + 1, simdstring2.begin() + 3).c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  // test concat
  EXPECT_STREQ((string1 + string2).c_str(), (simdstring1 + simdstring2).c_str());
  EXPECT_STREQ((string1 + sampleString).c_str(), (simdstring1 + sampleString).c_str());
  EXPECT_STREQ((string1 + 'a').c_str(), (simdstring1 + 'a').c_str());
  EXPECT_STREQ((sampleString + string1).c_str(), (sampleString + simdstring1).c_str());
  EXPECT_STREQ(('a' + string1).c_str(), ('a'+ simdstring1).c_str());

  // Test rvalue concat
  EXPECT_STREQ((sampleString + std::string(sampleString)).c_str(), (sampleString + SIMDString<64>(sampleString)).c_str());
  EXPECT_STREQ(('a' + std::string(sampleString)).c_str(), ('a'+ SIMDString<64>(sampleString)).c_str());
  EXPECT_STREQ((string1 + std::string(sampleString)).c_str(), (simdstring1 + SIMDString<64>(sampleString)).c_str());
  EXPECT_STREQ((std::string(sampleString) + string2).c_str(), (SIMDString<64>(sampleString) + simdstring2).c_str());
  EXPECT_STREQ((std::string(sampleString) + sampleString).c_str(), (SIMDString<64>(sampleString) + sampleString).c_str());
  EXPECT_STREQ((std::string(sampleString) + 'a').c_str(), (SIMDString<64>(sampleString) + 'a').c_str());
  EXPECT_STREQ((std::string(sampleString) + std::string(sampleString)).c_str(), (SIMDString<64>(sampleString) + SIMDString<64>(sampleString)).c_str());

  // append using input iterator
  std::istringstream ss1{sampleString};
  std::istreambuf_iterator<char> it1{ss1};
  std::istringstream ss2{sampleString};
  std::istreambuf_iterator<char> it2{ss2};

  string1.append(it1, std::istreambuf_iterator<char>());
  simdstring1.append(it2, std::istreambuf_iterator<char>());
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
}

TEST(SIMDStringTest, PushPopBack)
{
  SIMDString<64> simdstring1(sampleString);
  std::string string1(sampleString);

  string1.push_back('a');
  simdstring1.push_back('a');

  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  string1.pop_back();
  simdstring1.pop_back();
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  SIMDString<64> simdstring2;
  simdstring2.push_back('a');
  EXPECT_STREQ(simdstring2.c_str(), "a");
  EXPECT_EQ(simdstring2.length(), 1);
}

TEST(SIMDStringTest, Insert)
{
  std::string string1("0123456789");
  std::string string2("abcdefg");

  SIMDString<64> simdstring1("0123456789");
  SIMDString<64> simdstring2("abcdefg");

  #define INSERT_TESTS(stdPos, simdPos)\
  EXPECT_STREQ(string1.insert(stdPos, string2).c_str(), simdstring1.insert(simdPos, simdstring2).c_str());\
  EXPECT_EQ(string1.length(), simdstring1.length());\
  EXPECT_STREQ(string1.insert(stdPos, string2, 2, 3).c_str(), simdstring1.insert(simdPos, simdstring2, 2, 3).c_str());\
  EXPECT_EQ(string1.length(), simdstring1.length());\
  EXPECT_STREQ(string1.insert(stdPos, "hijklmnop").c_str(), simdstring1.insert(simdPos, "hijklmnop").c_str());\
  EXPECT_EQ(string1.length(), simdstring1.length());\
  EXPECT_STREQ(string1.insert(stdPos, "hijklmnop", 4).c_str(), simdstring1.insert(simdPos, "hijklmnop", 4).c_str());\
  EXPECT_EQ(string1.length(), simdstring1.length());\
  EXPECT_STREQ(string1.insert(stdPos, 6, 'z').c_str(), simdstring1.insert(simdPos, 6, 'z').c_str());\
  EXPECT_EQ(string1.length(), simdstring1.length());\

  // insert in the beginning
  INSERT_TESTS(0, 0)

  // insert in the middle 
  INSERT_TESTS(string1.size() / 2, simdstring1.size() / 2)

  // insert near the end
  INSERT_TESTS(string1.size() - 1, simdstring1.size() - 1)
  
  // insert at the end
  INSERT_TESTS(string1.size(), simdstring1.size())

  // insert using iterators
  string1.insert(string1.begin() + 2, string2.begin() + 2, string2.begin() + 4);
  simdstring1.insert(simdstring1.begin() + 2, simdstring2.begin() + 2, simdstring2.begin() + 4);
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  string1.insert(string1.begin(), '/');
  simdstring1.insert(simdstring1.begin(), '/');
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  // insert using input iterator
  std::istringstream ss1{sampleString};
  std::istreambuf_iterator<char> it1{ss1};
  std::istringstream ss2{sampleString};
  std::istreambuf_iterator<char> it2{ss2};

  string1.insert(string1.begin() + 6, it1, std::istreambuf_iterator<char>());
  simdstring1.insert(simdstring1.begin() + 6, it2, std::istreambuf_iterator<char>());
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
}

TEST(SIMDStringTest, Replace)
{
  std::string string1("0123456789");
  std::string string2("abcdefg");

  SIMDString<64> simdstring1("0123456789");
  SIMDString<64> simdstring2("abcdefg");
  
  std::string string3("abcdefg");
  SIMDString<64> simdstring3("abcdefg");

  #define REPLACE_TESTS(stdPos, simdPos, count, count2)\
  EXPECT_STREQ(string1.replace(stdPos, count, string2).c_str(), simdstring1.replace(simdPos, count, simdstring2).c_str());\
  EXPECT_EQ(string1.length(), simdstring1.length());\
  EXPECT_STREQ(string1.replace(stdPos, count, string2, 2, count2).c_str(), simdstring1.replace(simdPos, count, simdstring2, 2, count2).c_str());\
  EXPECT_EQ(string1.length(), simdstring1.length());\
  EXPECT_STREQ(string1.replace(stdPos, count, "hijklmnop").c_str(), simdstring1.replace(simdPos, count, "hijklmnop").c_str());\
  EXPECT_EQ(string1.length(), simdstring1.length());\
  EXPECT_STREQ(string1.replace(stdPos, count, "hijklmnop", count2).c_str(), simdstring1.replace(simdPos, count, "hijklmnop", count2).c_str());\
  EXPECT_EQ(string1.length(), simdstring1.length());\
  EXPECT_STREQ(string1.replace(stdPos, count, count2, 'z').c_str(), simdstring1.replace(simdPos, count, count2, 'z').c_str());\
  EXPECT_EQ(string1.length(), simdstring1.length());

  // replace at beginning, grow
  REPLACE_TESTS(0, 0, 1, 4)
  // replace at beginning, shrink
  REPLACE_TESTS(0, 0, 10, 4)
  // replace at beginning, same size
  REPLACE_TESTS(0, 0, 5, 5)

  // replace at middle, grow
  REPLACE_TESTS(5, 5, 1, 4)
  // replace at middle, shrink
  REPLACE_TESTS(5, 5, 10, 4)
  // replace at middle, same size
  REPLACE_TESTS(5, 5, 5, 5)

  // replace near the end, grow
  REPLACE_TESTS(string1.size() - 1, simdstring1.size() - 1, 1, 4)
  // replace near the end, smaller size, should still grow
  REPLACE_TESTS(string1.size() - 1, simdstring1.size() - 1, 10, 4)
  // replace near the end, same size, should still grow
  REPLACE_TESTS(string1.size() - 1, simdstring1.size() - 1, 5, 5)

  // replace at the end, grow
  REPLACE_TESTS(string1.size(), simdstring1.size(), 1, 4)
  // replace at the end, shrink
  REPLACE_TESTS(string1.size(), simdstring1.size(), 10, 4)
  // replace at the end, same size
  REPLACE_TESTS(string1.size(), simdstring1.size(), 5, 5)

  // replace buffer to heap
  EXPECT_STREQ(string3.replace(0, string1.size(), string1).c_str(), simdstring3.replace(0, simdstring1.size(), simdstring1).c_str());
  EXPECT_EQ(string3.length(), simdstring3.length());

  // replace using iterators, range
  string1.replace(string1.begin() + 2, string1.begin() + 6, string2.begin() + 2, string2.begin() + 4);
  simdstring1.replace(simdstring1.begin() + 2, simdstring1.begin() + 6, simdstring2.begin() + 2, simdstring2.begin() + 4);
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  // replace using iterators, str
  string1.replace(string1.begin() + 2, string1.begin() + 6, string2);
  simdstring1.replace(simdstring1.begin() + 2, simdstring1.begin() + 6, simdstring2);
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  // replace using iterators, cstr
  string1.replace(string1.begin() + 2, string1.begin() + 6, "hijklmnop");
  simdstring1.replace(simdstring1.begin() + 2, simdstring1.begin() + 6, "hijklmnop");
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  string1.replace(string1.begin() + 2, string1.begin() + 6, "hijklmnop", 3);
  simdstring1.replace(simdstring1.begin() + 2, simdstring1.begin() + 6, "hijklmnop", 3);
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  // replace using iterators, char
  string1.replace(string1.begin() + 2, string1.begin() + 6, 6, 'z');
  simdstring1.replace(simdstring1.begin() + 2, simdstring1.begin() + 6, 6, 'z');
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());

  // replace using input iterator
  std::istringstream ss1{sampleString};
  std::istreambuf_iterator<char> it1{ss1};
  std::istringstream ss2{sampleString};
  std::istreambuf_iterator<char> it2{ss2};

  string1.replace(string1.begin() + 2, string1.begin() + 6, it1, std::istreambuf_iterator<char>());
  simdstring1.replace(simdstring1.begin() + 2, simdstring1.begin() + 6, it2, std::istreambuf_iterator<char>());
  EXPECT_STREQ(string1.c_str(), simdstring1.c_str());
  EXPECT_EQ(string1.length(), simdstring1.length());
}

TEST(SIMDStringTest, ClearErase)
{
  SIMDString<64> simdstring1(sampleString);
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

  SIMDString<64> simdstring2(sampleString);
  simdstring2.clear();
  EXPECT_EQ(simdstring2.size(), 0);
  EXPECT_EQ(simdstring2.length(), 0);
  EXPECT_STREQ(simdstring2.c_str(), "");
  EXPECT_TRUE(simdstring2.empty());

  SIMDString<64> simdstring3(sampleString);
  std::string string3(sampleString);
  simdstring3.erase(10, 1);
  string3.erase(10, 1);
  EXPECT_EQ(simdstring3.size(), string3.size());
  EXPECT_STREQ(simdstring3.c_str(), string3.c_str());
  EXPECT_FALSE(simdstring3.empty());
  
  std::string string4(100, 'a');
  SIMDString<64> simdstring4(100, 'a');
  string4[75] = 'b';
  simdstring4[75] = 'b';
  EXPECT_EQ(simdstring4.size(), string4.size());
  EXPECT_STREQ(simdstring4.c_str(), string4.c_str());
  EXPECT_FALSE(simdstring4.empty());
}

TEST(SIMDStringTest, Swap)
{
  // buffer alloc
  std::string string1(20, 'a');
  SIMDString<64> simdstring1(20, 'a');

  // const string
  std::string string2(sampleString);
  SIMDString<64> simdstring2(sampleString);

  // heap alloc
  std::string string3(100, 'b');
  SIMDString<64> simdstring3(100, 'b');

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

  SIMDString<64> simdstring4(SIMDString<64>(26, 'z'));
  SIMDString<64> simdstring5(SIMDString<64>(sampleString));
  SIMDString<64> simdstring6(SIMDString<64>(200, 'z'));
}

TEST(SIMDStringTest, Find)
{
  std::string string1(findTestString);
  SIMDString<64> simdstring1(findTestString);
  std::string substr("abc");
  SIMDString<64> subsimdstr("abc"); 

  // find substring
  EXPECT_EQ(string1.find("abc"), simdstring1.find("abc"));
  EXPECT_EQ(string1.find("abcd"), simdstring1.find("abcd"));
  EXPECT_EQ(string1.find(substr), simdstring1.find(subsimdstr));
  // find substring with pos
  EXPECT_EQ(string1.find("abc", 10), simdstring1.find("abc", 10));
  EXPECT_EQ(string1.find("abcd", 10), simdstring1.find("abcd", 10));
  EXPECT_EQ(string1.find(substr, 10), simdstring1.find(subsimdstr, 10));
  // find substring with pos (abcd should not be found)
  EXPECT_EQ(string1.find("abc", 30), simdstring1.find("abc", 30));
  EXPECT_EQ(string1.find("abcd", 30), simdstring1.find("abcd", 30));
  EXPECT_EQ(string1.find(substr, 30), simdstring1.find(subsimdstr, 30));

  // find char (4 is no match)
  EXPECT_EQ(string1.find('a'), simdstring1.find('a'));
  EXPECT_EQ(string1.find('c'), simdstring1.find('c'));
  EXPECT_EQ(string1.find('d'), simdstring1.find('d'));
  EXPECT_EQ(string1.find('4'), simdstring1.find('4'));
  // find char with  (4 is no match)
  EXPECT_EQ(string1.find('a', 10), simdstring1.find('a', 10));
  EXPECT_EQ(string1.find('c', 10), simdstring1.find('c', 10));
  EXPECT_EQ(string1.find('d', 10), simdstring1.find('d', 10));
  EXPECT_EQ(string1.find('4', 10), simdstring1.find('4', 10));
  // find char with pos (d should not be found)
  EXPECT_EQ(string1.find('a', 30), simdstring1.find('a', 30));
  EXPECT_EQ(string1.find('c', 30), simdstring1.find('c', 30));
  EXPECT_EQ(string1.find('d', 30), simdstring1.find('d', 30));
  
  // find whole string
  EXPECT_EQ(string1.find(findTestString), simdstring1.find(findTestString));
  // find whole string (no match)
  EXPECT_EQ(string1.find(findTestString2), simdstring1.find(findTestString2));
  EXPECT_EQ(string1.find(findTestString3), simdstring1.find(findTestString3));

  // buffer alloc
  std::string string2(20, 'a');
  SIMDString<64> simdstring2(20, 'a');
  string2[15] = 'b';
  simdstring2[15] = 'b';
  string2.resize(11);
  simdstring2.resize(11);  

  // test edge case where b is definitely in an immediate, greater memory location
  EXPECT_EQ(string2.find('b', 6), simdstring2.find('b', 6));
  EXPECT_EQ(string2.find("ab", 6), simdstring2.find("ab", 6));
  EXPECT_EQ(string2.find('b', string2.size()), simdstring2.find('b', simdstring2.size()));
  EXPECT_EQ(string2.find("ab", string2.size()), simdstring2.find("ab", simdstring2.size()));
  // test empty string
  EXPECT_EQ(string2.find(""), simdstring2.find(""));
  EXPECT_EQ(string2.find("", 6), simdstring2.find("", 6));
  EXPECT_EQ(string2.find("", string2.size()), simdstring2.find("", simdstring2.size()));

  // test empty string
  SIMDString<64> simdstring3; 
  std::string string3; 
  EXPECT_EQ(string3.find('a'), simdstring3.find('a'));
  EXPECT_EQ(string3.find("abcd"), simdstring3.find("abcd"));
  
  // edge case null char
  SIMDString<64> simdstring4("test");
  simdstring4 += '\0';
  simdstring4 += "null";
  std::string string4("test");
  string4 += '\0';
  string4 += "null";
  EXPECT_EQ(string4.find('\0'), simdstring4.find('\0'));
}

TEST(SIMDStringTest, RFind)
{
  std::string string1(findTestString);
  SIMDString<64> simdstring1(findTestString);
  std::string substr("abc");
  SIMDString<64> subsimdstr("abc"); 

  // find substring
  EXPECT_EQ(string1.rfind("abc"), simdstring1.rfind("abc"));
  EXPECT_EQ(string1.rfind("abcd"), simdstring1.rfind("abcd"));
  EXPECT_EQ(string1.rfind(substr), simdstring1.rfind(subsimdstr));
  // find substring with pos
  EXPECT_EQ(string1.rfind("abc", 10), simdstring1.rfind("abc", 10));
  EXPECT_EQ(string1.rfind("abcd", 10), simdstring1.rfind("abcd", 10));
  EXPECT_EQ(string1.rfind(substr, 10), simdstring1.rfind(subsimdstr, 10));
  // find substring with pos (abcd should not be found)
  EXPECT_EQ(string1.rfind("abc", 30), simdstring1.rfind("abc", 30));
  EXPECT_EQ(string1.rfind("abcd", 30), simdstring1.rfind("abcd", 30));
  EXPECT_EQ(string1.rfind(substr, 30), simdstring1.rfind(subsimdstr, 30));

  // find char
  EXPECT_EQ(string1.rfind('c'), simdstring1.rfind('c'));
  EXPECT_EQ(string1.rfind('d'), simdstring1.rfind('d'));
  EXPECT_EQ(string1.rfind('A'), simdstring1.rfind('A'));
  // find char with pos
  EXPECT_EQ(string1.rfind('c', 10), simdstring1.rfind('c', 10));
  EXPECT_EQ(string1.rfind('d', 10), simdstring1.rfind('d', 10));
  EXPECT_EQ(string1.rfind('A', 10), simdstring1.rfind('A', 10));
  // find char with pos (d should not be found)
  EXPECT_EQ(string1.rfind('c', 30), simdstring1.rfind('c', 30));
  EXPECT_EQ(string1.rfind('d', 30), simdstring1.rfind('d', 30));
  EXPECT_EQ(string1.rfind('A', 30), simdstring1.rfind('A', 30));
  
  // find whole string
  EXPECT_EQ(string1.rfind(findTestString), simdstring1.rfind(findTestString));
  // find whole string (no match)
  EXPECT_EQ(string1.rfind(findTestString2), simdstring1.rfind(findTestString2));
  EXPECT_EQ(string1.rfind(findTestString3), simdstring1.rfind(findTestString3));

  std::string string2(findTestString, 30); 
  SIMDString<64> simdstring2(findTestString, 30);
  // test edge case where match is nearby but out of scope
  EXPECT_EQ(string2.rfind('d', 6), simdstring2.rfind('d', 6));
  EXPECT_EQ(string2.rfind("cd", 6), simdstring2.rfind("cd", 6));
  EXPECT_EQ(string2.rfind('d', string2.size()), simdstring2.rfind('d', simdstring2.size()));
  EXPECT_EQ(string2.rfind("cd", string2.size()), simdstring2.rfind("cd", simdstring2.size()));

  // test match empty string
  EXPECT_EQ(string2.rfind(""), simdstring2.rfind(""));
  EXPECT_EQ(string2.rfind("", 6), simdstring2.rfind("", 6));
  EXPECT_EQ(string2.rfind("", string2.size()), simdstring2.rfind("", simdstring2.size()));

  // test empty string
  SIMDString<64> simdstring3; 
  std::string string3; 
  EXPECT_EQ(string3.rfind('a'), simdstring3.rfind('a'));
  EXPECT_EQ(string3.rfind("abcd"), simdstring3.rfind("abcd"));
  
  // edge case null char
  SIMDString<64> simdstring4("test");
  simdstring4 += '\0';
  simdstring4 += "null";
  std::string string4("test");
  string4 += '\0';
  string4 += "null";
  EXPECT_EQ(string4.rfind('\0'), simdstring4.rfind('\0'));
}

TEST(SIMDStringTest, FindFirstLastOf)
{
  const char *findFirstLastOfTestString = "The quick brown fox jumps over the lazy dog. Sphinx of black quartz, judge my vow.";
  std::string string1(findFirstLastOfTestString);
  SIMDString<64> simdstring1(findFirstLastOfTestString);

  // find first/last of
  EXPECT_EQ(string1.find_first_of('d'), simdstring1.find_first_of('d'));
  EXPECT_EQ(string1.find_first_of("mnop"), simdstring1.find_first_of("mnop"));
  EXPECT_EQ(string1.find_last_of('d'), simdstring1.find_last_of('d'));
  EXPECT_EQ(string1.find_last_of("mnop"), simdstring1.find_last_of("mnop"));

  // find first/last of, with pos
  EXPECT_EQ(string1.find_first_of('d', 40), simdstring1.find_first_of('d', 40));
  EXPECT_EQ(string1.find_first_of("mnop", 40), simdstring1.find_first_of("mnop", 40));
  EXPECT_EQ(string1.find_last_of('d', 40), simdstring1.find_last_of('d', 40));
  EXPECT_EQ(string1.find_last_of("mnop", 40), simdstring1.find_last_of("mnop", 40));

  // find edge case, match == pos
  EXPECT_EQ(string1.find_first_of('q', string1.find('q')), simdstring1.find_first_of('q', simdstring1.find('q')));
  EXPECT_EQ(string1.find_first_of("mnop", string1.find('m')), simdstring1.find_first_of("mnop", simdstring1.find('m')));
  EXPECT_EQ(string1.find_last_of('d', string1.find('d')), simdstring1.find_last_of('d', simdstring1.find('d')));
  EXPECT_EQ(string1.find_last_of("jkl", string1.find('j')), simdstring1.find_last_of("jkl",simdstring1.find('j')));

  // find first/last of, no match
  EXPECT_EQ(string1.find_first_of('?'), simdstring1.find_first_of('?'));
  EXPECT_EQ(string1.find_first_of("1234"), simdstring1.find_first_of("1234"));
  EXPECT_EQ(string1.find_last_of('?'), simdstring1.find_last_of('?'));
  EXPECT_EQ(string1.find_last_of("1234"), simdstring1.find_last_of("1234"));

  // find first/last of with pos, no match
  EXPECT_EQ(string1.find_first_of('?', 90), simdstring1.find_first_of('?', 90));
  EXPECT_EQ(string1.find_first_of("1234", 90), simdstring1.find_first_of("1234", 90));
  EXPECT_EQ(string1.find_last_of('?', 90), simdstring1.find_last_of('?', 90));
  EXPECT_EQ(string1.find_last_of("1234", 90), simdstring1.find_last_of("1234", 90));

  // find first/last of, empty match string
  EXPECT_EQ(string1.find_first_of(""), simdstring1.find_first_of(""));
  EXPECT_EQ(string1.find_last_of(""), simdstring1.find_last_of(""));
  EXPECT_EQ(string1.find_first_of("", 90), simdstring1.find_first_of("", 90));
  EXPECT_EQ(string1.find_last_of("", 90), simdstring1.find_last_of("", 90));
  
  // find first/last of, edge case  
  size_t testStringSize = strlen(findFirstLastOfTestString);
  EXPECT_EQ(string1.find_first_of('d', testStringSize), simdstring1.find_first_of('d', testStringSize));
  EXPECT_EQ(string1.find_first_of("mnop", testStringSize), simdstring1.find_first_of("mnop", testStringSize));
  EXPECT_EQ(string1.find_last_of('d', testStringSize), simdstring1.find_last_of('d', testStringSize));
  EXPECT_EQ(string1.find_last_of("mnop", testStringSize), simdstring1.find_last_of("mnop", testStringSize));

  // buffer alloc
  std::string string2(20, 'a');
  SIMDString<64> simdstring2(20, 'a');
  string2[15] = 'b';
  simdstring2[15] = 'b';
  string2.resize(11);
  simdstring2.resize(11);  

  // test edge case where match is nearby but out of scope
  EXPECT_EQ(string2.find_first_of('b'), simdstring2.find_first_of('b'));
  EXPECT_EQ(string2.find_first_of("bc"), simdstring2.find_first_of("bc"));
  EXPECT_EQ(string2.find_last_of('b'), simdstring2.find_last_of('b'));
  EXPECT_EQ(string2.find_last_of("bc"), simdstring2.find_last_of("bc"));
  EXPECT_EQ(string2.find_first_of('b', 6), simdstring2.find_first_of('b', 6));
  EXPECT_EQ(string2.find_first_of("bc", 6), simdstring2.find_first_of("bc", 6));
  EXPECT_EQ(string2.find_last_of('b', 6), simdstring2.find_last_of('b', 6));
  EXPECT_EQ(string2.find_last_of("bc", 6), simdstring2.find_last_of("bc", 6));

  SIMDString<64> simdstring3; 
  std::string string3; 
  // test empty string
  EXPECT_EQ(string3.find_first_of('d'), simdstring3.find_first_of('d'));
  EXPECT_EQ(string3.find_first_of("mnop"), simdstring3.find_first_of("mnop"));
  EXPECT_EQ(string3.find_last_of('d'), simdstring3.find_last_of('d'));
  EXPECT_EQ(string3.find_last_of("mnop"), simdstring3.find_last_of("mnop"));
}

TEST(SIMDStringTest, FindFirstLastNotOf)
{
  const char *findFirstLastNotOfTestString = "     a           b     m       c            d            e    t     f         ";
  std::string string1(findFirstLastNotOfTestString);
  SIMDString<64> simdstring1(findFirstLastNotOfTestString);

  // find first/last not of (space in "abcdef " is deliberate) 
  EXPECT_EQ(string1.find_first_not_of(' '), simdstring1.find_first_not_of(' '));
  EXPECT_EQ(string1.find_first_not_of("abcdef "), simdstring1.find_first_not_of("abcdef "));
  EXPECT_EQ(string1.find_last_not_of(' '), simdstring1.find_last_not_of(' '));
  EXPECT_EQ(string1.find_last_not_of("abcdef "), simdstring1.find_last_not_of("abcdef "));

  // find first/last not of with pos
  EXPECT_EQ(string1.find_first_not_of(' ', 35), simdstring1.find_first_not_of(' ', 35));
  EXPECT_EQ(string1.find_first_not_of("abcdef ", 35), simdstring1.find_first_not_of("abcdef ", 35));
  EXPECT_EQ(string1.find_last_not_of(' ', 35), simdstring1.find_last_not_of(' ', 35));
  EXPECT_EQ(string1.find_last_not_of("abcdef ", 35), simdstring1.find_last_not_of("abcdef ", 35));

  // find edge case, match == pos
  EXPECT_EQ(string1.find_first_not_of(' ', string1.find('a')), simdstring1.find_first_not_of(' ', simdstring1.find('a')));
  EXPECT_EQ(string1.find_first_not_of("abcdef ", string1.find('t')), simdstring1.find_first_not_of("abcdef ", simdstring1.find('t')));
  EXPECT_EQ(string1.find_last_not_of(' ', string1.find('f')), simdstring1.find_last_not_of(' ', simdstring1.find('f')));
  EXPECT_EQ(string1.find_last_not_of("abcdef ", string1.find('t')), simdstring1.find_last_not_of("abcdef ", simdstring1.find('t')));
  
  // find first/last not of, no match
  EXPECT_EQ(string1.find_first_not_of("abcdefmt "), simdstring1.find_first_not_of("abcdefmt "));
  EXPECT_EQ(string1.find_last_not_of("abcdefmt "), simdstring1.find_last_not_of("abcdefmt "));
  EXPECT_EQ(string1.substr(0, 3).find_first_not_of(' '), simdstring1.substr(0, 3).find_first_not_of(' '));
  EXPECT_EQ(string1.substr(0, 3).find_last_not_of(' '), simdstring1.substr(0, 3).find_last_not_of(' '));

  // find first/last not of, no match with pos
  EXPECT_EQ(string1.find_first_not_of("abcdeft ", 35), simdstring1.find_first_not_of("abcdeft ", 35));
  EXPECT_EQ(string1.find_last_not_of("abcdeft ", 35), simdstring1.find_last_not_of("abcdeft ", 35));
  EXPECT_EQ(string1.find_first_not_of(' ', string1.find('f') + 1), simdstring1.find_first_not_of(' ', simdstring1.find('f') + 1));
  EXPECT_EQ(string1.find_last_not_of(' ', string1.find('f') + 1), simdstring1.find_last_not_of(' ', simdstring1.find('f') + 1));

  // find first/last of, empty match string
  EXPECT_EQ(string1.find_first_not_of(""), simdstring1.find_first_not_of(""));
  EXPECT_EQ(string1.find_last_not_of(""), simdstring1.find_last_not_of(""));
  EXPECT_EQ(string1.find_first_not_of("", 35), simdstring1.find_first_not_of("", 35));
  EXPECT_EQ(string1.find_last_not_of("", 35), simdstring1.find_last_not_of("", 35));
  
  // find first/last of, edge case  
  size_t testStringSize = strlen(findFirstLastNotOfTestString);
  EXPECT_EQ(string1.find_first_not_of('?', testStringSize), simdstring1.find_first_not_of('?', testStringSize));
  EXPECT_EQ(string1.find_first_not_of("1234", testStringSize), simdstring1.find_first_not_of("1234", testStringSize));
  EXPECT_EQ(string1.find_last_not_of('?', testStringSize), simdstring1.find_last_not_of('?', testStringSize));
  EXPECT_EQ(string1.find_last_not_of("1234", testStringSize), simdstring1.find_last_not_of("1234", testStringSize));

  // buffer alloc
  std::string string2(20, 'a');
  SIMDString<64> simdstring2(20, 'a');
  string2[2] = 'c';
  simdstring2[2] = 'c';
  string2[15] = 'b';
  simdstring2[15] = 'b';
  string2.resize(11);
  simdstring2.resize(11);  

  // test edge case where match is nearby but out of scope
  EXPECT_EQ(string2.find_first_not_of('a'), simdstring2.find_first_not_of('a'));
  EXPECT_EQ(string2.find_first_not_of("ac"), simdstring2.find_first_not_of("ac"));
  EXPECT_EQ(string2.find_last_not_of('a'), simdstring2.find_last_not_of('a'));
  EXPECT_EQ(string2.find_last_not_of("ac"), simdstring2.find_last_not_of("ac"));
  EXPECT_EQ(string2.find_first_not_of('a', 6), simdstring2.find_first_not_of('a', 6));
  EXPECT_EQ(string2.find_first_not_of("ac", 6), simdstring2.find_first_not_of("ac", 6));
  EXPECT_EQ(string2.find_last_not_of('a', 6), simdstring2.find_last_not_of('a', 6));
  EXPECT_EQ(string2.find_last_not_of("ac", 6), simdstring2.find_last_not_of("ac", 6));

  SIMDString<64> simdstring3; 
  std::string string3; 
  // test empty string
  EXPECT_EQ(string3.find_first_not_of('d'), simdstring3.find_first_not_of('d'));
  EXPECT_EQ(string3.find_first_not_of("mnop"), simdstring3.find_first_not_of("mnop"));
  EXPECT_EQ(string3.find_last_not_of('d'), simdstring3.find_last_not_of('d'));
  EXPECT_EQ(string3.find_last_not_of("mnop"), simdstring3.find_last_not_of("mnop"));

  SIMDString<64> simdstring4("test");
  simdstring4 += '\0';
  simdstring4 += "null";
  
  std::string string4("test");
  string4 += '\0';
  string4 += "null";
}

TEST(SIMDStringTest, StartsEndsWith)
{
  SIMDString<64> simdstring1(sampleString);

  EXPECT_TRUE(simdstring1.starts_with("the"));
  EXPECT_FALSE(simdstring1.starts_with("woah"));
  EXPECT_TRUE(simdstring1.ends_with("dog"));
  EXPECT_FALSE(simdstring1.ends_with("woah"));
  EXPECT_TRUE(simdstring1.starts_with('t'));
  EXPECT_FALSE(simdstring1.starts_with('w'));
  EXPECT_TRUE(simdstring1.ends_with('g'));
  EXPECT_FALSE(simdstring1.ends_with('h'));
  EXPECT_TRUE(simdstring1.starts_with(sampleString));
  EXPECT_TRUE(simdstring1.ends_with(sampleString));

  const char* s = "hello world";
  EXPECT_FALSE(SIMDString(s + 6).ends_with("hello world"));
}

TEST(SIMDStringTest, Contains)
{
  SIMDString<64> simdstring1(sampleString);

  EXPECT_TRUE(simdstring1.contains("the"));
  EXPECT_FALSE(simdstring1.contains("woah"));
  EXPECT_FALSE(simdstring1.contains('T'));
}

TEST(SIMDStringTest, CopySubstr)
{
  const char* copySubstrTestString = "0123456789abcdefghijklmnopqrstuvwxyz";
  char tmp[11];
  SIMDString<64> simdstring1(copySubstrTestString);
  tmp[10] = '\0';

  // test copy and substr for first 10 chars
  EXPECT_EQ(10, simdstring1.copy(tmp, 10));
  EXPECT_STREQ(tmp, "0123456789");
  SIMDString<64> substr = simdstring1.substr(0, 10);
  EXPECT_EQ(strlen(tmp), substr.size());
  EXPECT_STREQ(tmp, substr.c_str());

  // test copy and substr for chars [10, 20)
  EXPECT_EQ(10, simdstring1.copy(tmp, 10, 10));
  EXPECT_STREQ(tmp, "abcdefghij");
  substr = simdstring1.substr(10, 10);
  EXPECT_EQ(strlen(tmp), substr.size());
  EXPECT_STREQ(tmp, substr.c_str());

  // test substr for [10, end of string)
  substr = simdstring1.substr(10);
  EXPECT_EQ(strlen(copySubstrTestString + 10), substr.size());
  EXPECT_STREQ(copySubstrTestString + 10, substr.c_str());

  // Test substr with null chars
  SIMDString<64> simdstring2("test");
  simdstring2 += '\0';
  simdstring2 += "null";
  simdstring2 += '\0';
  simdstring2 += "ability";
  
  // STREQ is for const char* only, which we can't test with null chars
  // So look for each word individually and test size
  substr = simdstring2.substr(5);
  EXPECT_STREQ("null", substr.c_str());
  EXPECT_STREQ("ability", substr.c_str() + 5);
  EXPECT_EQ(simdstring2.size() - 5, substr.size());
  
  substr = simdstring2.substr(10);
  EXPECT_STREQ("ability", substr.c_str());
  EXPECT_EQ(simdstring2.size() - 10, substr.size());
}

TEST(SIMDStringTest, SizeClear)
{
  SIMDString<64> simdstring1(sampleString);
  std::string string1(sampleString);

  // resize bigger, don't set  
  simdstring1.resize(50);
  string1.resize(50);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  // resize bigger, do set
  simdstring1.resize(60, '!');
  string1.resize(60, '!');
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  // resize smaller
  simdstring1.resize(40);
  string1.resize(40);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  // resize smaller, do set
  simdstring1.resize(30, '!');
  string1.resize(30, '!');
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  // test length (size is used extensively in these tests so we assume it's working right)  
  EXPECT_EQ(simdstring1.length(), string1.length());

  // test empty (false)
  EXPECT_FALSE(simdstring1.empty());

  // test clear/empty
  simdstring1.clear();
  EXPECT_TRUE(simdstring1.empty());
}

TEST(SIMDStringTest, Conversions)
{
  SIMDString<64> simdstring1 = to_string(1234567890);
  std::string string1 = std::to_string(1234567890);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(1234567890u);
  string1 = std::to_string(1234567890u);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(1234567890l);
  string1 = std::to_string(1234567890l);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(1234567890ul);
  string1 = std::to_string(1234567890ul);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(1234567890ll);
  string1 = std::to_string(1234567890ll);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(1234567890ull);
  string1 = std::to_string(1234567890ull);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(INT_MAX);
  string1 = std::to_string(INT_MAX);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(INT_MIN);
  string1 = std::to_string(INT_MIN);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(LONG_MAX);
  string1 = std::to_string(LONG_MAX);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(LONG_MIN);
  string1 = std::to_string(LONG_MIN);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(LLONG_MAX);
  string1 = std::to_string(LLONG_MAX);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(LLONG_MIN);
  string1 = std::to_string(LLONG_MIN);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(UINT_MAX);
  string1 = std::to_string(UINT_MAX);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(ULONG_MAX);
  string1 = std::to_string(ULONG_MAX);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(ULLONG_MAX);
  string1 = std::to_string(ULLONG_MAX);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());


  simdstring1 = to_string(0X1.BC70A3D70A3D7P+6f);
  string1 = std::to_string(0X1.BC70A3D70A3D7P+6f);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(0X1.BC70A3D70A3D7P+6);
  string1 = std::to_string(0X1.BC70A3D70A3D7P+6);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(0X1.BC70A3D70A3D7P+6l);
  string1 = std::to_string(0X1.BC70A3D70A3D7P+6l);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(123456789012345678901234567890.12345678901234567890123456789012345678901234567890123456789012345678901234567890f);
  string1 = std::to_string(123456789012345678901234567890.12345678901234567890123456789012345678901234567890123456789012345678901234567890f);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  simdstring1 = to_string(123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567891234567890.12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789);
  string1 = std::to_string(123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567891234567890.12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());
  
  simdstring1 = to_string(123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567891234567890.1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789L);
  string1 = std::to_string(123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567891234567890.1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789L);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(simdstring1.size(), string1.size());

  SIMDString<64> simdstring2("-1234567890");
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

  SIMDString<64> simdstring3("-1.52343249");
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
  std::istringstream iss3("Hello there!\nWho are you?");
  std::istringstream iss4("Hello there!\nWho are you?");

  std::string tmp(1 << 21, '-');
  std::istringstream iss5(tmp);
  std::istringstream iss6(tmp);
  std::istringstream iss7(tmp);
  std::istringstream iss8(tmp);

  SIMDString<64> simdstring1;
  std::string string1;

  // test operator>>
  iss1 >> simdstring1;
  iss2 >> string1;
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  // test getline
  std::getline(iss1, string1);
  getline(iss2, simdstring1);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  // test format for input  
  iss3.setf(std::ios::skipws);
  iss4.setf(std::ios::skipws);
  iss3 >> simdstring1;
  iss4 >> string1;
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());
  EXPECT_EQ(iss3.get(), iss4.get());

  iss7 >> simdstring1;
  iss8 >> string1;
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  std::getline(iss5, string1);
  getline(iss6, simdstring1);
  EXPECT_STREQ(simdstring1.c_str(), string1.c_str());

  // test operator<<
  std::ostringstream oss2;
  std::ostringstream oss1;

  SIMDString<64> simdstring2(sampleString);
  std::string string2(sampleString);

  oss1 << std::setw(50) << simdstring2 << std::setfill('*') << std::setw(50) << simdstring2 << std::endl; 
  oss2 << std::setw(50) << string2 << std::setfill('*') << std::setw(50) << string2 << std::endl; 

  EXPECT_STREQ(oss1.str().c_str(), oss2.str().c_str());
}

TEST(SIMDStringTest, Reserve){
  size_t largeSize = 1 << 21; 
  // test that reserve allocates exactly how much is expected + 1
  SIMDString<64> simdstring1;
  simdstring1.reserve(largeSize);
  EXPECT_LE(largeSize, simdstring1.capacity());

  // call to reserve with smaller numbers doesn't do anything
  simdstring1.reserve();
  EXPECT_LE(largeSize, simdstring1.capacity());
  simdstring1.reserve(largeSize / 2);
  EXPECT_LE(largeSize, simdstring1.capacity());

  // call to reserve doesn't change expected size
  simdstring1 = SIMDString<64>(100, 'a');
  simdstring1.reserve();
  EXPECT_LE(100, simdstring1.capacity());

  // in buffer, call to reserve with no params, should preserve buffer size
  simdstring1 = SIMDString<64>(10, 'a');
  simdstring1.reserve();
  EXPECT_EQ(64, simdstring1.capacity());
  EXPECT_STREQ("aaaaaaaaaa", simdstring1.c_str());

  // in buffer, call to reserve with larger size, data should move to heap
  simdstring1 = SIMDString<64>(10, 'a');
  simdstring1.reserve(largeSize);
  EXPECT_LE(largeSize, simdstring1.capacity());
  EXPECT_STREQ("aaaaaaaaaa", simdstring1.c_str());

  // when inConst, capacity is exactly as big as the const string
  simdstring1 = SIMDString<64>(sampleString);
  EXPECT_EQ(sampleStringSize, simdstring1.capacity());
  simdstring1.reserve();
  EXPECT_EQ(sampleStringSize, simdstring1.capacity());
  
  // test that reserve allocates expected amount AND that const string is copied over
  simdstring1 = SIMDString<64>(sampleString);
  simdstring1.reserve(largeSize);
  EXPECT_LE(largeSize, simdstring1.capacity());
  EXPECT_STREQ(sampleString, simdstring1.c_str());
}

TEST(SIMDStringTest, Hash){
  SIMDString<64> simdstring1(sampleString);
  std::string string1(sampleString);

  // only works if G3D Alloc is disabled bc had to hard code in types for hash
  size_t simdHash = std::hash<SIMDString<64>>{}(simdstring1);
  size_t stringHash = std::hash<std::string>{}(string1);

  EXPECT_EQ(simdHash, stringHash);
}

TEST(SIMDStringTest, RangeLoops){
  std::string result1, result2;

  SIMDString<64> simdstring1(sampleString);
  std::string string1(sampleString);

  for (char c : simdstring1){
    result1.push_back(c);
  }

  for (char c : string1){
    result2.push_back(c);
  }

  EXPECT_STREQ(result1.c_str(), result2.c_str());
}