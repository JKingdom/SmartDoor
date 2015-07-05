
#ifndef __BRD_MD5_H__
#define __BRD_MD5_H__

#include <string>
#include <iostream>

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif
#ifndef uint8_t
typedef unsigned char uint8_t; //  8bit
#endif

#define  blocksize 64

class CMD5
{
public:
	CMD5();
	CMD5(const std::string& text);
	
	void		CalcMd5(const char* input, uint32_t length);
	void		CalcMd5(const unsigned char* input, uint32_t length);
	CMD5&		Finalize();
	std::string GetMd5() const;
	friend std::ostream& operator<<(std::ostream&, CMD5 md5);

private:
	uint8_t			m_ui8Buffer[blocksize]; // bytes that didn't fit in last 64 byte chunk
	uint32_t		m_ui32Count[2];   // 64bit counter for number of bits (lo, hi)
	uint32_t		m_ui32State[4];   // m_ui8Digest so far
	uint8_t			m_ui8Digest[16]; // the result

	void				init();
	void				calculate(const unsigned char* input, uint32_t length);
	void				transform(const uint8_t block[blocksize]);
	static void			decode(uint32_t output[], const uint8_t input[], uint32_t len);
	static void			encode(uint8_t output[], const uint32_t input[], uint32_t len);

	static inline uint32_t RotateLeft(uint32_t x, int n);
	static inline void	FF(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
	static inline void	GG(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
	static inline void	HH(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
	static inline void	II(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
};

std::string md5(const std::string str);

#endif