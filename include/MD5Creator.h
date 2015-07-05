/*
 * MD5Creator.h
 *
 *  Created on: 2015年5月30日
 *      Author: xqf
 */

#ifndef SRC_MD5CREATOR_H_
#define SRC_MD5CREATOR_H_

/*
 for loop A\B\C\D ways
*/
#define F(x,y,z) (((x)&(y))|((~x)&(z)))
#define G(x,y,z) (((x)&(z))|((y)&(~z)))
#define H(x,y,z) ((x)^(y)^(z))
#define I(x,y,z) ((y)^((x)|(~z)))

#define ROT(x,s) (x=(x<<s)|(x>>(32-s)))

#define FF(a,b,c,d,j,s,T) {a=a+(F(b,c,d)+M[j]+T);ROT(a,s);a=a+b;}
#define GG(a,b,c,d,j,s,T) {a=a+(G(b,c,d)+M[j]+T);ROT(a,s);a=a+b;}
#define HH(a,b,c,d,j,s,T) {a=a+(H(b,c,d)+M[j]+T);ROT(a,s);a=a+b;}
#define II(a,b,c,d,j,s,T) {a=a+(I(b,c,d)+M[j]+T);ROT(a,s);a=a+b;}

namespace std {
typedef unsigned int md5_int;
struct MD5_struct {
	md5_int A;
	md5_int B;
	md5_int C;
	md5_int D;
	md5_int lenbuf;
	char buffer[128];
};
static struct MD5_struct ctx;
static md5_int M[16];
static md5_int fullbuffer[64];
class MD5Creator {
public:
	MD5Creator(void *rebuf);
	~MD5Creator();

private:
	/* set data for fill buffer */
	void md5_init( char * buffer);
	void md5_process();
	void md5_fini( void *rebuf);
	void md5_buffer_full();
	void md5_print();
	void md5_create(void *rebuf);
};

} /* namespace std */

#endif /* SRC_MD5CREATOR_H_ */
