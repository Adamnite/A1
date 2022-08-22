/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */
#include "CoreUtils/Utf8.hpp"

namespace CoreUtils{
    
    //Check against UTF8 parameters for validity
    
    bool isCorrect(char _x, char _y){
		short x = (short) _x;
		short y = (short) _y;
        if (x == 0xc0 || x == 0xc1){
            return false;
        }
        else if (x >= 0xc2 && y <= 0xdf){
            return true;
        
        }
        else if (x == 0xe0)
        {
            if (y < 0xa0){
                return false;
            }
            else {
                return true;
            }

            }
            else if (x >= 0xe1 && x <= 0xec)
		return true;
	else if (x == 0xed)
	{
		if (y > 0x9f)
			return false;
		else
			return true;
	}
	else if (x == 0xee || x == 0xef)
		return true;
	else if (x == 0xf0)
	{
		if (y < 0x90)
			return false;
		else
			return true;
	}
	else if (x >= 0xf1 && x <= 0xf3)
		return true;
	else if (x == 0xf4)
	{
		if (y > 0x8f)
			return false;
		else
			return true;
	}
	return false;//pyro: adding this catch case, shouldn't be called, but makes the compiler happy
    }

    bool isUTFencoding(unsigned char const* _input, size_t _length, size_t& position){
		// it appears position is just changed if there is an error, but i may be mistaken
		bool valid = true;
		size_t i = 0;

		for (; i < _length; i++)
		{
			// Check for Unicode Chapter 3 Table 3-6 conformity.
			if (_input[i] < 0x80)
				continue;

			size_t count = 0;
			if (_input[i] >= 0xc0 && _input[i] <= 0xdf)
				count = 1;
			else if (_input[i] >= 0xe0 && _input[i] <= 0xef)
				count = 2;
			else if (_input[i] >= 0xf0 && _input[i] <= 0xf7)
				count = 3;

			if (count == 0)
			{
				valid = false;
				break;
			}

			if ((i + count) >= _length)
			{
				valid = false;
				break;
			}

			for (size_t j = 0; j < count; j++)
			{
				i++;
				if ((_input[i] & 0xc0) != 0x80)
				{
					valid = false;
					break;
				}

				// Check for Unicode Chapter 3 Table 3-7 conformity.
				if ((j == 0) && !isCorrect(_input[i - 1], _input[i]))
				{
					valid = false;
					break;
				}
			}
		}

		if (valid)
			return true;

		position = i;
		return false;
	}

	bool isUTFencoding(std::string const& _input, size_t& position){
		return isUTFencoding(reinterpret_cast<unsigned char const*>(_input.c_str()), _input.length(), position);
	}
}
