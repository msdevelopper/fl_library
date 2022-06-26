
#include "fl.h"
int 	indexof(char *_arr[], size_t _sz, char *_s)
{
	int _i = 0;
	int _len = _sz / sizeof(_arr[0]);
	while (_i < _len)
	{
		if (!strcmp(_s, _arr[_i])) return _i;
		//if (!memcmp(_s, _arr[_i], strlen(_arr[_i]))) return _i;
		_i++;
	}
	return -1;
}
int 	char_indexof(char _arr[], size_t _sz, char _s)
{
	int _i = 0;
	int _len = _sz / sizeof(_arr[0]);
	while (_i < _len)
	{
		if (_s == _arr[_i]) return _i;
		_i++;
	}
	return -1;
}
