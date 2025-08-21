#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/String.h"

TEST(String, PushBack) {
	constexpr int valsToAdd = 10;

	BIEngine::String str;

	{
		char ch = '0';
		for (int i = 0; i < valsToAdd; ++i, ++ch) {
			str.PushBack(ch);
		}
	}

	{
		char ch = '0';
		for (int i = 0; i < valsToAdd; ++i, ++ch) {
			EXPECT_EQ(str[i], ch);
		}
	}
}

TEST(String, Reserve) {
	constexpr int valsToAdd = 10;

	BIEngine::String str;
	str.Reserve(valsToAdd);

	EXPECT_EQ(str.Capacity(), valsToAdd);

	{
		char ch = '0';
		for (int i = 0; i < valsToAdd; ++i) {
			str.PushBack(ch);
		}
	}

	EXPECT_EQ(str.Capacity(), valsToAdd);
}

TEST(String, ModifyElement) {
	constexpr int valsToAdd = 10;
	constexpr char testVal = 'a';

	BIEngine::String str;

	{
		char ch = '0';
		for (int i = 0; i < valsToAdd; ++i) {
			str.PushBack(ch);
		}
	}

	str[0] = testVal;

	EXPECT_EQ(str[0], testVal);
}

TEST(String, CapacityChanging) {
	constexpr int valsToAdd = 10;

	BIEngine::String str;
	const BIEngine::SizeT initialCapacity = str.Capacity();

	{
		char ch = '0';
		for (int i = 0; i < valsToAdd; ++i) {
			str.PushBack(ch);
		}
	}

	EXPECT_TRUE(initialCapacity < str.Size() < str.Capacity());
}

TEST(String, CopyCtor) {
	constexpr int valsNum = 10;

	BIEngine::String str1;
	
	{
		char ch = '0';
		for (int i = 0; i < valsNum; ++i, ++ch) {
			str1.PushBack(ch);
		}
	}

	const BIEngine::String str2(str1);

	{
		char ch = '0';
		for (int i = 0; i < valsNum; ++i, ++ch) {
			EXPECT_EQ(str2[i], ch);
		}
	}
}

TEST(String, CopyAssignment) {
	constexpr int valsNum = 10;

	BIEngine::String str1;

	{
		char ch = '0';
		for (int i = 0; i < valsNum; ++i, ++ch) {
			str1.PushBack(ch);
		}
	}

	const BIEngine::String str2 = str1;

	{
		char ch = '0';
		for (int i = 0; i < valsNum; ++i, ++ch) {
			EXPECT_EQ(str2[i], ch);
		}
	}
}

TEST(String, MoveCtor) {
	constexpr int valsNum = 10;

	BIEngine::String str1;

	{
		char ch = '0';
		for (int i = 0; i < valsNum; ++i, ++ch) {
			str1.PushBack(ch);
		}
	}

	const BIEngine::String str2(std::move(str1));

	{
		char ch = '0';
		for (int i = 0; i < valsNum; ++i, ++ch) {
			EXPECT_EQ(str2[i], ch);
		}
	}
}

TEST(String, MoveAssignment) {
	constexpr int valsNum = 10;

	BIEngine::String str2;

	{
		BIEngine::String str1;

		{
			char ch = '0';
			for (int i = 0; i < valsNum; ++i, ++ch) {
				str1.PushBack(ch);
			}
		}

		str2 = std::move(str1);
	}

	{
		char ch = '0';
		for (int i = 0; i < valsNum; ++i, ++ch) {
			EXPECT_EQ(str2[i], ch);
		}
	}
}

TEST(String, NCtor) {
	constexpr int valsToAdd = 10;

	BIEngine::String str(valsToAdd);

	EXPECT_TRUE(valsToAdd == str.Size());

	for (int i = 0; i < str.Size(); ++i) {
		EXPECT_EQ(str[i], '\0');
	}
}

TEST(String, NCopyCtor) {
	constexpr int numVals = 10;
	constexpr char valToAdd = 'b';

	BIEngine::String str(numVals, valToAdd);

	for (int i = 0; i < numVals; ++i) {
		EXPECT_EQ(str[i], valToAdd);
	}

	EXPECT_TRUE(numVals == str.Capacity() && numVals == str.Size());
}

TEST(String, CtorCStr) {
	const char* rawStr = "test string for string";
	int rawStrSize = 0;
	while (rawStr[rawStrSize++] != '\0')
	{

	}
	--rawStrSize;

	BIEngine::String str(rawStr);

	EXPECT_EQ(rawStrSize, str.Size());

	for (int i = 0; i < str.Size(); ++i) {
		EXPECT_EQ(str[i], rawStr[i]);
	}
}

TEST(String, NCtorCStr) {
	const char* rawStr = "test string for string";
	constexpr int nToAdd = 11;

	BIEngine::String str(rawStr, nToAdd);

	EXPECT_EQ(nToAdd, str.Size());

	for (int i = 0; i < str.Size(); ++i) {
		EXPECT_EQ(str[i], rawStr[i]);
	}
}

TEST(String, CtorSprintf) {
	BIEngine::String str(BIEngine::String::CtorSprintf(), "my new %d string", 12345);
	EXPECT_EQ(str.Size(), 19);

	const char* expectedStrRaw = "my new 12345 string";
	for (int i = 0; i < str.Size(); ++i)
	{
		EXPECT_EQ(str[i], expectedStrRaw[i]);
	}
}

TEST(String, EqPlusOperatorChar) {
	constexpr char chToAdd = 'b';

	BIEngine::String str;

	str += chToAdd;

	EXPECT_EQ(str.Size(), 1);
	EXPECT_EQ(str[0], chToAdd);
}

TEST(String, EqPlusOperatorRawStr) {
	const char* rawStr = "test string for string";
	int rawStrSize = 0;
	while (rawStr[rawStrSize++] != '\0')
	{

	}
	--rawStrSize;

	BIEngine::String str;

	str += rawStr;

	EXPECT_EQ(str.Size(), rawStrSize);

	for (int i = 0; i < str.Size(); ++i) {
		EXPECT_EQ(str[i], rawStr[i]);
	}
}

TEST(String, EqPlusOperatorStr) {
	const char* rawStr = "test string for string";

	BIEngine::String strToAdd(rawStr);
	BIEngine::String str;

	str += strToAdd;

	EXPECT_EQ(str.Size(), strToAdd.Size());

	for (int i = 0; i < str.Size(); ++i) {
		EXPECT_EQ(str[i], strToAdd[i]);
	}
}

TEST(String, InitializerList) {

	BIEngine::String str{ '0','1','2','3','4' };

	for (int i = 0; i < 5; ++i) {
		EXPECT_EQ(str[i], '0' + i);
	}
}

TEST(String, Iterators) {
	constexpr int valsNum = 10;

	BIEngine::String str;

	{
		char ch = '0';
		for (int i = 0; i < valsNum; ++i, ++ch) {
			str.PushBack(ch);
		}
	}

	{
		char i = '0';
		for (BIEngine::String::Iterator itr = str.Begin(); itr != str.End(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
		}
	}

	{
		char i = '9';
		for (BIEngine::String::Iterator itr = str.Begin(); itr != str.End(); ++itr, --i) {
			*itr = i;
		}
	}

	{
		char i = '9';
		for (BIEngine::String::ConstIterator itr = str.CBegin(); itr != str.CEnd(); ++itr, --i) {
			EXPECT_EQ(*itr, i);
		}
	}
}

TEST(String, PopBack) {
	constexpr int valsNum = 10;

	BIEngine::String str;

	{
		char ch = '0';
		for (int i = 0; i < valsNum; ++i, ++ch) {
			str.PushBack(ch);
		}
	}

	str.PopBack();

	EXPECT_EQ(str.Size(), valsNum - 1);

	{
		char i = '0';
		for (auto itr = str.CBegin(); itr != str.CEnd(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
		}
	}
}

TEST(String, Insert) {
	constexpr int valsNum = 10;

	BIEngine::String str;

	{
		char ch = '0';
		for (int i = 0; i < valsNum; ++i, ++ch) {
			str.PushBack(ch);
		}
	}

	constexpr BIEngine::SizeT testIdx = 5;
	constexpr char testVal = 'b';

	str.Insert(str.Begin() + testIdx, testVal);

	{
		char i = '0';
		for (auto itr = str.CBegin(); itr != str.CEnd(); ++itr, ++i) {
			if (itr - str.CBegin() == testIdx) {
				EXPECT_EQ(*itr, testVal);
				--i;
				continue;
			}

			EXPECT_EQ(*itr, i);
		}
	}
}

TEST(String, AppendVSprintf) {
	BIEngine::String str;
	str.AppendSprintf("my new %d string", 12345);

	EXPECT_EQ(str.Size(), 19);

	const char* expectedStrRaw = "my new 12345 string";
	for (int i = 0; i < str.Size(); ++i)
	{
		EXPECT_EQ(str[i], expectedStrRaw[i]);
	}
}

TEST(String, Erase) {
	constexpr int valsNum = 10;

	BIEngine::String str;

	{
		char ch = '0';
		for (int i = 0; i < valsNum; ++i, ++ch) {
			str.PushBack(ch);
		}
	}

	constexpr BIEngine::SizeT testIdx = 5;

	str.Erase(str.Begin() + testIdx);

	{
		char i = '0';
		for (auto itr = str.CBegin(); itr != str.CEnd(); ++itr, ++i) {
			if (itr - str.CBegin() == testIdx) {
				++i;
			}

			EXPECT_EQ(*itr, i);
		}
	}
}

TEST(String, ResizeGrow) {
	constexpr int valsNum = 10;
	constexpr int valsAfterResizeNum = 20;

	BIEngine::String str;

	{
		char ch = '0';
		for (int i = 0; i < valsNum; ++i, ++ch) {
			str.PushBack(ch);
		}
	}

	str.Resize(valsAfterResizeNum);

	EXPECT_EQ(str.Size(), valsAfterResizeNum);

	{
		char i = '0';
		for (auto itr = str.CBegin(); itr != str.CEnd(); ++itr, ++i) {
			if (itr - str.CBegin() >= valsNum) {
				EXPECT_EQ(*itr, int());
				continue;
			}

			EXPECT_EQ(*itr, i);
		}
	}
}

TEST(String, ResizeShrink) {
	constexpr int valsNum = 10;
	constexpr int valsAfterResizeNum = 5;

	BIEngine::String str;

	{
		char ch = '0';
		for (int i = 0; i < valsNum; ++i, ++ch) {
			str.PushBack(ch);
		}
	}

	str.Resize(valsAfterResizeNum);

	EXPECT_EQ(str.Size(), valsAfterResizeNum);

	{
		char i = '0';
		for (auto itr = str.CBegin(); itr != str.CEnd(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
		}
	}
}

TEST(String, GlobalPlusOperator_Str_Str) {
	BIEngine::String str1("one");
	BIEngine::String str2("two");

	BIEngine::String str = str1 + str2;

	EXPECT_EQ(str.Size(), str1.Size() + str2.Size());

	for (int i = 0; i < str1.Size() + str2.Size(); ++i) {
		if (i < str1.Size()) {
			EXPECT_EQ(str[i], str1[i]);
			continue;
		}

		EXPECT_EQ(str[i], str2[i - str1.Size()]);
	}
}

TEST(String, GlobalPlusOperator_Str_RawStr) {
	BIEngine::String str1("one");
	const char* rawStr1 = "two";

	BIEngine::String str = str1 + rawStr1;

	EXPECT_EQ(str.Size(), str1.Size() + 3);

	for (int i = 0; i < str1.Size() + 3; ++i) {
		if (i < str1.Size()) {
			EXPECT_EQ(str[i], str1[i]);
			continue;
		}

		EXPECT_EQ(str[i], rawStr1[i - str1.Size()]);
	}
}

TEST(String, GlobalPlusOperator_RawStr_Str) {
	BIEngine::String str1("one");
	const char* rawStr1 = "two";

	BIEngine::String str = rawStr1 + str1;

	EXPECT_EQ(str.Size(), str1.Size() + 3);

	for (int i = 0; i < str1.Size() + 3; ++i) {
		if (i < 3) {
			EXPECT_EQ(str[i], rawStr1[i]);
			continue;
		}

		EXPECT_EQ(str[i], str1[i - 3]);
	}
}

TEST(String, GlobalEqOperator_Str_RawStr) {
	const char* rawStr1 = "one";
	BIEngine::String str1(rawStr1);
	const char* rawStr2 = "two";

	EXPECT_TRUE(str1 == rawStr1);
	EXPECT_FALSE(str1 == rawStr2);
}

TEST(String, GlobalNEqOperator_Str_RawStr) {
	const char* rawStr1 = "one";
	BIEngine::String str1(rawStr1);
	const char* rawStr2 = "two";

	EXPECT_TRUE(str1 != rawStr2);
	EXPECT_FALSE(str1 != rawStr1);
}

TEST(String, GlobalEqOperator_RawStr_Str) {
	const char* rawStr1 = "one";
	BIEngine::String str1(rawStr1);
	const char* rawStr2 = "two";

	EXPECT_TRUE(rawStr1 == str1);
	EXPECT_FALSE(rawStr2 == str1);
}

TEST(String, GlobalNEqOperator_RawStr_Str) {
	const char* rawStr1 = "one";
	BIEngine::String str1(rawStr1);
	const char* rawStr2 = "two";

	EXPECT_TRUE(rawStr2 != str1);
	EXPECT_FALSE(rawStr1 != str1);
}

TEST(String, GlobalEqOperator_Str_Str) {
	BIEngine::String str1("one");
	BIEngine::String str2("one");
	BIEngine::String str3("two");

	EXPECT_TRUE(str1 == str2);
	EXPECT_FALSE(str1 == str3);
}

TEST(String, GlobalNEqOperator_Str_Str) {
	BIEngine::String str1("one");
	BIEngine::String str2("one");
	BIEngine::String str3("two");

	EXPECT_TRUE(str1 != str3);
	EXPECT_FALSE(str1 != str2);
}

TEST(String, Find) {
	BIEngine::String str("1234512345");

	BIEngine::SizeT pos1 = str.Find('3');
	BIEngine::SizeT pos2 = str.Find('6');
	BIEngine::SizeT pos3 = str.Find('3', pos1 + 1);

	EXPECT_EQ(pos1, 2);
	EXPECT_EQ(pos2, BIEngine::String::NPos);
	EXPECT_EQ(pos3, 7);
}

TEST(String, RFind) {
	BIEngine::String str("1234512345");

	BIEngine::SizeT pos1 = str.RFind('3');
	BIEngine::SizeT pos2 = str.RFind('6');
	BIEngine::SizeT pos3 = str.RFind('3', pos1 - 1);

	EXPECT_EQ(pos1, 7);
	EXPECT_EQ(pos2, BIEngine::String::NPos);
	EXPECT_EQ(pos3, 2);
}

TEST(String, FindRawStr) {
	BIEngine::String str("1234512345");

	BIEngine::SizeT pos1 = str.Find("451");
	BIEngine::SizeT pos2 = str.Find("666");
	BIEngine::SizeT pos3 = str.Find("123", pos1 + 1);
	BIEngine::SizeT pos4 = str.Find("1234512345");

	EXPECT_EQ(pos1, 3);
	EXPECT_EQ(pos2, BIEngine::String::NPos);
	EXPECT_EQ(pos3, 5);
	EXPECT_EQ(pos4, 0);
}

TEST(String, Substr) {
	BIEngine::String str("0123456789");

	BIEngine::String substr1 = str.Substr();
	BIEngine::String substr2 = str.Substr(4);
	BIEngine::String substr3 = str.Substr(4, 3);
	BIEngine::String substr4 = str.Substr(7, 10);

	EXPECT_STREQ(substr1.CStr(), "0123456789");
	EXPECT_STREQ(substr2.CStr(), "456789");
	EXPECT_STREQ(substr3.CStr(), "456");
	EXPECT_STREQ(substr4.CStr(), "789");
}

TEST(String, Getline) {
	std::istringstream is("abc\n123");
	BIEngine::String str1;
	BIEngine::String str2;

	BIEngine::Getline(is, str1);
	BIEngine::Getline(is, str2);

	EXPECT_STREQ(str1.CStr(), "abc");
	EXPECT_STREQ(str2.CStr(), "123");
}