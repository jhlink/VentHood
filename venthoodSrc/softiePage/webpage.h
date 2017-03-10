#include "FSEmeric-Regular.h"
#include "FSEmeric-Medium.h"

// To any future maintainers. Sorry. Code must be minified for the sake of RAM.
// Bright side, you'll never need to touch these files.
const char jsbn_1_js[] = "var dbits;var canary=0xdeadbeefcafe;var j_lm=((canary&0xffffff)==0xefcafe);function BigInteger(a,b,c){if(a!=null)if('number'==typeof a)this.fromNumber(a,b,c);else if(b==null&&'string'!=typeof a)this.fromString(a,256);else this.fromString(a,b);}function nbi(){return new BigInteger(null);}function am1(a,b,c,d,e,f){while(--f>=0){var g=b*this[a++]+c[d]+e;e=Math.floor(g/0x4000000);c[d++]=g&0x3ffffff;}return e;}function am2(a,b,c,d,e,f){var g=b&0x7fff,h=b>>15;while(--f>=0){var i=this[a]&0x7fff;var j=this[a++]>>15;var k=h*i+j*g;i=g*i+((k&0x7fff)<<15)+c[d]+(e&0x3fffffff);e=(i>>>30)+(k>>>15)+h*j+(e>>>30);c[d++]=i&0x3fffffff;}return e;}function am3(a,b,c,d,e,f){var g=b&0x3fff,h=b>>14;while(--f>=0){var i=this[a]&0x3fff;var j=this[a++]>>14;var k=h*i+j*g;i=g*i+((k&0x3fff)<<14)+c[d]+e;e=(i>>28)+(k>>14)+h*j;c[d++]=i&0xfffffff;}return e;}if(j_lm&&(navigator.appName=='Microsoft Internet Explorer')){BigInteger.prototype.am=am2;dbits=30;}else if(j_lm&&(navigator.appName!='Netscape')){BigInteger.prototype.am=am1;dbits=26;}else{BigInteger.prototype.am=am3;dbits=28;}BigInteger.prototype.DB=dbits;BigInteger.prototype.DM=((1<<dbits)-1);BigInteger.prototype.DV=(1<<dbits);var BI_FP=52;BigInteger.prototype.FV=Math.pow(2,BI_FP);BigInteger.prototype.F1=BI_FP-dbits;BigInteger.prototype.F2=2*dbits-BI_FP;var BI_RM='0123456789abcdefghijklmnopqrstuvwxyz';var BI_RC=new Array();var rr,vv;rr='0'.charCodeAt(0);for(vv=0;vv<=9;++vv)BI_RC[rr++]=vv;rr='a'.charCodeAt(0);for(vv=10;vv<36;++vv)BI_RC[rr++]=vv;rr='A'.charCodeAt(0);for(vv=10;vv<36;++vv)BI_RC[rr++]=vv;function int2char(a){return BI_RM.charAt(a);}function intAt(a,b){var c=BI_RC[a.charCodeAt(b)];return(c==null)?-1:c;}function bnpCopyTo(a){for(var b=this.t-1;b>=0;--b)a[b]=this[b];a.t=this.t;a.s=this.s;}function bnpFromInt(a){this.t=1;this.s=(a<0)?-1:0;if(a>0)this[0]=a;else if(a<-1)this[0]=a+this.DV;else this.t=0;}function nbv(a){var b=nbi();b.fromInt(a);return b;}function bnpFromString(a,b){var c;if(b==16)c=4;else if(b==8)c=3;else if(b==256)c=8;else if(b==2)c=1;else if(b==32)c=5;else if(b==4)c=2;else{this.fromRadix(a,b);return;}this.t=0;this.s=0;var d=a.length,e=false,f=0;while(--d>=0){var g=(c==8)?a[d]&0xff:intAt(a,d);if(g<0){if(a.charAt(d)=='-')e=true;continue;}e=false;if(f==0)this[this.t++]=g;else if(f+c>this.DB){this[this.t-1]|=(g&((1<<(this.DB-f))-1))<<f;this[this.t++]=(g>>(this.DB-f));}else this[this.t-1]|=g<<f;f+=c;if(f>=this.DB)f-=this.DB;}if(c==8&&(a[0]&0x80)!=0){this.s=-1;if(f>0)this[this.t-1]|=((1<<(this.DB-f))-1)<<f;}this.clamp();if(e)BigInteger.ZERO.subTo(this,this);}function bnpClamp(){var a=this.s&this.DM;while(this.t>0&&this[this.t-1]==a)--this.t;}function bnToString(a){if(this.s<0)return '-'+this.negate().toString(a);var b;if(a==16)b=4;else if(a==8)b=3;else if(a==2)b=1;else if(a==32)b=5;else if(a==4)b=2;else return this.toRadix(a);var c=(1<<b)-1,d,e=false,f='',g=this.t;var h=this.DB-(g*this.DB)%b;if(g-->0){if(h<this.DB&&(d=this[g]>>h)>0){e=true;f=int2char(d);}while(g>=0){if(h<b){d=(this[g]&((1<<h)-1))<<(b-h);d|=this[--g]>>(h+=this.DB-b);}else{d=(this[g]>>(h-=b))&c;if(h<=0){h+=this.DB;--g;}}if(d>0)e=true;if(e)f+=int2char(d);}}return e?f:'0';}function bnNegate(){var a=nbi();BigInteger.ZERO.subTo(this,a);return a;}function bnAbs(){return(this.s<0)?this.negate():this;}function bnCompareTo(a){var b=this.s-a.s;if(b!=0)return b;var c=this.t;b=c-a.t;if(b!=0)return(this.s<0)?-b:b;while(--c>=0)if((b=this[c]-a[c])!=0)return b;return 0;}function nbits(a){var b=1,c;if((c=a>>>16)!=0){a=c;b+=16;}if((c=a>>8)!=0){a=c;b+=8;}if((c=a>>4)!=0){a=c;b+=4;}if((c=a>>2)!=0){a=c;b+=2;}if((c=a>>1)!=0){a=c;b+=1;}return b;}function bnBitLength(){if(this.t<=0)return 0;return this.DB*(this.t-1)+nbits(this[this.t-1]^(this.s&this.DM));}function bnpDLShiftTo(a,b){var c;for(c=this.t-1;c>=0;--c)b[c+a]=this[c];for(c=a-1;c>=0;--c)b[c]=0;b.t=this.t+a;b.s=this.s;}function bnpDRShiftTo(a,b){for(var c=a;c<this.t;++c)b[c-a]=this[c];b.t=Math.max(this.t-a,0);b.s=this.s;}function bnpLShiftTo(a,b){var c=a%this.DB;var d=this.DB-c;var e=(1<<d)-1;var f=Math.floor(a/this.DB),g=(this.s<<c)&this.DM,h;for(h=this.t-1;h>=0;--h){b[h+f+1]=(this[h]>>d)|g;g=(this[h]&e)<<c;}for(h=f-1;h>=0;--h)b[h]=0;b[f]=g;b.t=this.t+f+1;b.s=this.s;b.clamp();}";
const char rsa_js[] = "function parseBigInt(a,b){return new BigInteger(a,b);}function linebrk(a,b){var c='';var d=0;while(d+b<a.length){c+=a.substring(d,d+b)+'\\n';d+=b;}return c+a.substring(d,a.length);}function byte2Hex(a){if(a<0x10)return '0'+a.toString(16);else return a.toString(16);}function pkcs1pad2(a,b){if(b<a.length+11){alert('Message too long for RSA');return null;}var c=new Array();var d=a.length-1;while(d>=0&&b>0){var e=a.charCodeAt(d--);if(e<128)c[--b]=e;else if((e>127)&&(e<2048)){c[--b]=(e&63)|128;c[--b]=(e>>6)|192;}else{c[--b]=(e&63)|128;c[--b]=((e>>6)&63)|128;c[--b]=(e>>12)|224;}}c[--b]=0;var f=new SecureRandom();var g=new Array();while(b>2){g[0]=0;while(g[0]==0)f.nextBytes(g);c[--b]=g[0];}c[--b]=2;c[--b]=0;return new BigInteger(c);}function RSAKey(){this.n=null;this.e=0;this.d=null;this.p=null;this.q=null;this.dmp1=null;this.dmq1=null;this.coeff=null;}function RSASetPublic(a,b){if(a!=null&&b!=null&&a.length>0&&b.length>0){this.n=parseBigInt(a,16);this.e=parseInt(b,16);}else alert('Invalid RSA public key');}function RSADoPublic(a){return a.modPowInt(this.e,this.n);}function RSAEncrypt(a){var b=pkcs1pad2(a,(this.n.bitLength()+7)>>3);if(b==null)return null;var c=this.doPublic(b);if(c==null)return null;var d=c.toString(16);if((d.length&1)==0)return d;else return '0'+d;}RSAKey.prototype.doPublic=RSADoPublic;RSAKey.prototype.setPublic=RSASetPublic;RSAKey.prototype.encrypt=RSAEncrypt;";
const char rng_js[] = "var rng_state;var rng_pool;var rng_pptr;function rng_seed_int(a){rng_pool[rng_pptr++]^=a&255;rng_pool[rng_pptr++]^=(a>>8)&255;rng_pool[rng_pptr++]^=(a>>16)&255;rng_pool[rng_pptr++]^=(a>>24)&255;if(rng_pptr>=rng_psize)rng_pptr-=rng_psize;}function rng_seed_time(){rng_seed_int(new Date().getTime());}if(rng_pool==null){rng_pool=new Array();rng_pptr=0;var t;if(window.crypto&&window.crypto.getRandomValues){var ua=new Uint8Array(32);window.crypto.getRandomValues(ua);for(t=0;t<32;++t)rng_pool[rng_pptr++]=ua[t];}if(navigator.appName=='Netscape'&&navigator.appVersion<'5'&&window.crypto){var z=window.crypto.random(32);for(t=0;t<z.length;++t)rng_pool[rng_pptr++]=z.charCodeAt(t)&255;}while(rng_pptr<rng_psize){t=Math.floor(65536*Math.random());rng_pool[rng_pptr++]=t>>>8;rng_pool[rng_pptr++]=t&255;}rng_pptr=0;rng_seed_time();}function rng_get_byte(){if(rng_state==null){rng_seed_time();rng_state=prng_newstate();rng_state.init(rng_pool);for(rng_pptr=0;rng_pptr<rng_pool.length;++rng_pptr)rng_pool[rng_pptr]=0;rng_pptr=0;}return rng_state.next();}function rng_get_bytes(a){var b;for(b=0;b<a.length;++b)a[b]=rng_get_byte();}function SecureRandom(){}SecureRandom.prototype.nextBytes=rng_get_bytes;";
const char jsbn_2_js[] = "function bnpRShiftTo(a,b){b.s=this.s;var c=Math.floor(a/this.DB);if(c>=this.t){b.t=0;return;}var d=a%this.DB;var e=this.DB-d;var f=(1<<d)-1;b[0]=this[c]>>d;for(var g=c+1;g<this.t;++g){b[g-c-1]|=(this[g]&f)<<e;b[g-c]=this[g]>>d;}if(d>0)b[this.t-c-1]|=(this.s&f)<<e;b.t=this.t-c;b.clamp();}function bnpSubTo(a,b){var c=0,d=0,e=Math.min(a.t,this.t);while(c<e){d+=this[c]-a[c];b[c++]=d&this.DM;d>>=this.DB;}if(a.t<this.t){d-=a.s;while(c<this.t){d+=this[c];b[c++]=d&this.DM;d>>=this.DB;}d+=this.s;}else{d+=this.s;while(c<a.t){d-=a[c];b[c++]=d&this.DM;d>>=this.DB;}d-=a.s;}b.s=(d<0)?-1:0;if(d<-1)b[c++]=this.DV+d;else if(d>0)b[c++]=d;b.t=c;b.clamp();}function bnpMultiplyTo(a,b){var c=this.abs(),d=a.abs();var e=c.t;b.t=e+d.t;while(--e>=0)b[e]=0;for(e=0;e<d.t;++e)b[e+c.t]=c.am(0,d[e],b,e,0,c.t);b.s=0;b.clamp();if(this.s!=a.s)BigInteger.ZERO.subTo(b,b);}function bnpSquareTo(a){var b=this.abs();var c=a.t=2*b.t;while(--c>=0)a[c]=0;for(c=0;c<b.t-1;++c){var d=b.am(c,b[c],a,2*c,0,1);if((a[c+b.t]+=b.am(c+1,2*b[c],a,2*c+1,d,b.t-c-1))>=b.DV){a[c+b.t]-=b.DV;a[c+b.t+1]=1;}}if(a.t>0)a[a.t-1]+=b.am(c,b[c],a,2*c,0,1);a.s=0;a.clamp();}function bnpDivRemTo(a,b,c){var d=a.abs();if(d.t<=0)return;var e=this.abs();if(e.t<d.t){if(b!=null)b.fromInt(0);if(c!=null)this.copyTo(c);return;}if(c==null)c=nbi();var f=nbi(),g=this.s,h=a.s;var i=this.DB-nbits(d[d.t-1]);if(i>0){d.lShiftTo(i,f);e.lShiftTo(i,c);}else{d.copyTo(f);e.copyTo(c);}var j=f.t;var k=f[j-1];if(k==0)return;var l=k*(1<<this.F1)+((j>1)?f[j-2]>>this.F2:0);var m=this.FV/l,n=(1<<this.F1)/l,o=1<<this.F2;var p=c.t,q=p-j,r=(b==null)?nbi():b;f.dlShiftTo(q,r);if(c.compareTo(r)>=0){c[c.t++]=1;c.subTo(r,c);}BigInteger.ONE.dlShiftTo(j,r);r.subTo(f,f);while(f.t<j)f[f.t++]=0;while(--q>=0){var s=(c[--p]==k)?this.DM:Math.floor(c[p]*m+(c[p-1]+o)*n);if((c[p]+=f.am(0,s,c,q,0,j))<s){f.dlShiftTo(q,r);c.subTo(r,c);while(c[p]<--s)c.subTo(r,c);}}if(b!=null){c.drShiftTo(j,b);if(g!=h)BigInteger.ZERO.subTo(b,b);}c.t=j;c.clamp();if(i>0)c.rShiftTo(i,c);if(g<0)BigInteger.ZERO.subTo(c,c);}function bnMod(a){var b=nbi();this.abs().divRemTo(a,null,b);if(this.s<0&&b.compareTo(BigInteger.ZERO)>0)a.subTo(b,b);return b;}function Classic(a){this.m=a;}function cConvert(a){if(a.s<0||a.compareTo(this.m)>=0)return a.mod(this.m);else return a;}function cRevert(a){return a;}function cReduce(a){a.divRemTo(this.m,null,a);}function cMulTo(a,b,c){a.multiplyTo(b,c);this.reduce(c);}function cSqrTo(a,b){a.squareTo(b);this.reduce(b);}Classic.prototype.convert=cConvert;Classic.prototype.revert=cRevert;Classic.prototype.reduce=cReduce;Classic.prototype.mulTo=cMulTo;Classic.prototype.sqrTo=cSqrTo;function bnpInvDigit(){if(this.t<1)return 0;var a=this[0];if((a&1)==0)return 0;var b=a&3;b=(b*(2-(a&0xf)*b))&0xf;b=(b*(2-(a&0xff)*b))&0xff;b=(b*(2-(((a&0xffff)*b)&0xffff)))&0xffff;b=(b*(2-a*b%this.DV))%this.DV;return(b>0)?this.DV-b:-b;}function Montgomery(a){this.m=a;this.mp=a.invDigit();this.mpl=this.mp&0x7fff;this.mph=this.mp>>15;this.um=(1<<(a.DB-15))-1;this.mt2=2*a.t;}function montConvert(a){var b=nbi();a.abs().dlShiftTo(this.m.t,b);b.divRemTo(this.m,null,b);if(a.s<0&&b.compareTo(BigInteger.ZERO)>0)this.m.subTo(b,b);return b;}function montRevert(a){var b=nbi();a.copyTo(b);this.reduce(b);return b;}function montReduce(a){while(a.t<=this.mt2)a[a.t++]=0;for(var b=0;b<this.m.t;++b){var c=a[b]&0x7fff;var d=(c*this.mpl+(((c*this.mph+(a[b]>>15)*this.mpl)&this.um)<<15))&a.DM;c=b+this.m.t;a[c]+=this.m.am(0,d,a,b,0,this.m.t);while(a[c]>=a.DV){a[c]-=a.DV;a[++c]++;}}a.clamp();a.drShiftTo(this.m.t,a);if(a.compareTo(this.m)>=0)a.subTo(this.m,a);}function montSqrTo(a,b){a.squareTo(b);this.reduce(b);}function montMulTo(a,b,c){a.multiplyTo(b,c);this.reduce(c);}Montgomery.prototype.convert=montConvert;Montgomery.prototype.revert=montRevert;Montgomery.prototype.reduce=montReduce;Montgomery.prototype.mulTo=montMulTo;Montgomery.prototype.sqrTo=montSqrTo;function bnpIsEven(){return((this.t>0)?(this[0]&1):this.s)==0;}function bnpExp(a,b){if(a>0xffffffff||a<1)return BigInteger.ONE;var c=nbi(),d=nbi(),e=b.convert(this),f=nbits(a)-1;e.copyTo(c);while(--f>=0){b.sqrTo(c,d);if((a&(1<<f))>0)b.mulTo(d,e,c);else{var g=c;c=d;d=g;}}return b.revert(c);}function bnModPowInt(a,b){var c;if(a<256||b.isEven())c=new Classic(b);else c=new Montgomery(b);return this.exp(a,c);}BigInteger.prototype.copyTo=bnpCopyTo;BigInteger.prototype.fromInt=bnpFromInt;BigInteger.prototype.fromString=bnpFromString;BigInteger.prototype.clamp=bnpClamp;BigInteger.prototype.dlShiftTo=bnpDLShiftTo;BigInteger.prototype.drShiftTo=bnpDRShiftTo;BigInteger.prototype.lShiftTo=bnpLShiftTo;BigInteger.prototype.rShiftTo=bnpRShiftTo;BigInteger.prototype.subTo=bnpSubTo;BigInteger.prototype.multiplyTo=bnpMultiplyTo;BigInteger.prototype.squareTo=bnpSquareTo;BigInteger.prototype.divRemTo=bnpDivRemTo;BigInteger.prototype.invDigit=bnpInvDigit;BigInteger.prototype.isEven=bnpIsEven;BigInteger.prototype.exp=bnpExp;BigInteger.prototype.toString=bnToString;BigInteger.prototype.negate=bnNegate;BigInteger.prototype.abs=bnAbs;BigInteger.prototype.compareTo=bnCompareTo;BigInteger.prototype.bitLength=bnBitLength;BigInteger.prototype.mod=bnMod;BigInteger.prototype.modPowInt=bnModPowInt;BigInteger.ZERO=nbv(0);BigInteger.ONE=nbv(1);";
const char prng4_js[] = "function Arcfour(){this.i=0;this.j=0;this.S=new Array();}function ARC4init(a){var b,c,d;for(b=0;b<256;++b)this.S[b]=b;c=0;for(b=0;b<256;++b){c=(c+this.S[b]+a[b%a.length])&255;d=this.S[b];this.S[b]=this.S[c];this.S[c]=d;}this.i=0;this.j=0;}function ARC4next(){var a;this.i=(this.i+1)&255;this.j=(this.j+this.S[this.i])&255;a=this.S[this.i];this.S[this.i]=this.S[this.j];this.S[this.j]=a;return this.S[(a+this.S[this.i])&255];}Arcfour.prototype.init=ARC4init;Arcfour.prototype.next=ARC4next;function prng_newstate(){return new Arcfour();}var rng_psize=256;";
const char OneBLogo_svg[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!-- Generator: Adobe Illustrator 20.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  --><svg version=\"1.1\" id=\"Layer_1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"	 viewBox=\"0 0 355.8 49.8\" style=\"enable-background:new 0 0 355.8 49.8;\" xml:space=\"preserve\"><style type=\"text/css\">	.st0{fill:#FF4612;}</style><g>	<g>		<path class=\"st0\" d=\"M27.4,38.8h30.8c1.7,0,3-0.2,3.9-0.6s1.3-1.2,1.3-2.5v-4.1c0-0.9-0.4-1.5-1.3-2c-0.9-0.4-2.1-0.6-3.7-0.6			H27.4L27.4,38.8L27.4,38.8z M27.4,21.2h31.5c1.6,0,2.8-0.2,3.5-0.5c0.7-0.4,1-0.9,1-1.5v-4.1c0-0.8-0.4-1.3-1.2-1.7			c-0.8-0.4-2-0.6-3.4-0.6H27.4V21.2z M19,47.3V4.4h39.5c4.6,0,7.9,0.7,10,2.2s3.2,3.7,3.2,6.8v6.1c0,1.5-0.4,2.8-1.3,3.7			c-0.8,0.9-1.9,1.6-3.1,2c1.3,0.4,2.4,1.1,3.2,2.1c0.8,0.9,1.2,2.4,1.2,4.2v6.9c0,2.8-1.1,5-3.3,6.6c-2.2,1.6-5.5,2.4-10.1,2.4H19			V47.3z\"/>	</g>	<polygon class=\"st0\" points=\"7.1,4.3 5.8,4.3 1.4,9.2 1.4,12.6 3.1,12.6 3.1,47.3 7.1,47.3 11.5,47.3 11.5,4.3 	\"/>	<polygon points=\"99,22.3 118.1,22.3 118.1,29.7 99,29.7 99,47.2 90,47.2 90,4.4 120.4,4.4 120.4,11.9 99,11.9 	\"/>	<g>		<path d=\"M124.1,15.7h8.3v31.5h-8.3V15.7z M124.1,2.4h6.6c1.4,0,1.7,0.5,1.7,1.6v6.6h-8.3L124.1,2.4L124.1,2.4z\"/>		<path d=\"M156,22.5c-3.2,0.6-5.9,1.4-8.3,2.1c-0.8,0.3-0.9,0.5-0.9,1.2v21.4h-8.3V15.7h6.3l1.3,3.9c4.1-3,6.9-4.4,9.1-4.4			c1.3,0,2.8,0.5,4,1.1V22L156,22.5z\"/>		<path d=\"M181.8,22.5c-3-0.3-5.6-0.5-7.3-0.5c-3.9,0-5.8,0.5-5.8,2.2c0,1.4,1.5,1.8,4,2.5l4.2,1c7.3,1.8,10.5,3.9,10.5,9.6			c0,6.1-4.4,10.4-13,10.4c-4.7,0-10.2-1.5-13.7-3.4l1.7-5.1l3.5,0.6c3.7,0.7,6.4,1,8.5,1c3.4,0,5.3-1,5.3-3c0-1.6-0.9-2.2-4.2-3			l-4.1-1c-6.4-1.6-10.4-3.9-10.4-9.4c0-6,5.5-9.3,13.3-9.3c4,0,8.4,0.9,11.8,2.5l-1.3,5.2L181.8,22.5z\"/>		<path d=\"M215.5,43.8c-2.7,2.1-7,3.9-10.7,3.9c-3.2,0-5.8-1-7.5-2.7c-1.9-1.9-2.8-4.5-2.8-8.2V22.1h-5.8v-5.2l5.7-1.2V6.5h8.3v9.1			h10.5v6.5h-10.5v14.4c0,1.4,0.2,2.3,0.8,2.9c0.6,0.6,1.5,1,2.8,1c1.1,0,2.8-0.3,4.1-0.6l3-0.6L215.5,43.8z\"/>		<path d=\"M228,39.7h10.4c5.2,0,7-1.6,7-5.4c0-3.6-1.4-5.1-6.1-5.1H228L228,39.7L228,39.7z M239.8,22.1c1,0,1.6-0.3,2.1-1.1			c0.7-1.1,1.2-2.7,1.2-4.3c0-3.8-1.8-4.8-6.5-4.8H228v10.3h11.7V22.1z M237.3,4.4c8.9,0,14.7,3.2,14.7,10.6c0,3.5-1.6,6.5-4,9.1			c3.7,2,6.7,5.4,6.7,10.6c0,8.3-5.4,12.5-15.6,12.5h-19.8V4.3L237.3,4.4L237.3,4.4z\"/>		<path d=\"M266.5,35.2c0,1.6,0.3,2.7,1,3.4c0.9,1,2.5,1.3,4.9,1.3c2.1,0,4.5-0.3,5.8-0.8c0.6-0.3,0.6-0.7,0.6-1.3V15.7h8.3v31.5			h-6.3l-1.1-2.7l-1.8,1c-2.9,1.6-5,2.3-7.5,2.3c-7.2,0-12.3-4.4-12.3-12.3V15.7h8.3L266.5,35.2L266.5,35.2z\"/>		<path d=\"M293.7,15.7h8.3v31.5h-8.3V15.7z M293.7,2.4h6.6c1.4,0,1.7,0.5,1.7,1.6v6.6h-8.3V2.4z\"/>		<path d=\"M322.2,45.9c-1.6,0.8-4.5,1.5-6.4,1.5c-2.2,0-4.1-0.8-5.3-2c-1.3-1.4-1.9-3.2-1.9-5.7V1.4h8.3v39l5.3,1.1L322.2,45.9			L322.2,45.9z\"/>	<path d=\"M346.2,25.5c0-0.6,0-0.9-0.6-1.2c-1.6-0.8-4-1.4-6.5-1.4c-4.8,0-7.4,2.2-7.4,8.5c0,6.5,2.6,8.6,7.4,8.6			c2.3,0,4.9-0.6,6.5-1.4c0.5-0.3,0.6-0.5,0.6-1.2V25.5z M346.2,1.4h8.3v45.8h-6.3l-1.2-2.9l-2.4,1.3c-2.5,1.4-4.8,2.1-7.4,2.1			c-8.3,0-14.2-6.5-14.2-16.2c0-9.5,6.1-16.4,14.2-16.4c3.1,0,5.9,1.2,8.9,3V1.4H346.2z\"/>	</g></g></svg>";

//  This is stuff that needs to be changed. There are online JS, CSS, HTML minimizers available. Utilize thy Google-Fu.
const char index_html[] = "<!DOCTYPE html><html> <head> <meta name='viewport' content='width=device-width, initial-scale=1'> <link rel='stylesheet' type='text/css' href='style.css'> <img src='1BLogo.svg' id='logo' alt='firstbuild'> </head> <body> <div id='progress-line' style='inline'> <hr class='style'> <span id='stage1' class='circle'></span> <span id='stage2' class='circle_open'></span> <span id='stage3' class='circle_open'></span> <span id='stage4' class='circle_open'></span> </div><div id='hood-device-div' style='display: block'> <h2>Name your Venthood<br>lights and fan.</h2> <form id='venthood-form'> <input type='text' id='light' size='25' placeholder='Venthood Light'/> <input type='text' id='fan' size='25' placeholder='Venthood Exhaust'/> <div id='email-req'>*Required</div><input type='text' id='email' size='25' placeholder='Amazon Email'/> <button type='submit' class='input-helper' id='update-button'>Submit</button> </form> </div><div id='scan-div' style='display: none'> <button type='button' class='input-helper' id='scan-button'>Find Your Home Network</button> </div><div id='networks-div'></div><div id='connect-div' style='display: none'> <form id='connect-form'> <div id='email-req'>*Required</div><input type='password' id='password' size='25' placeholder='Password'/> <button type='button' class='input-helper' id='show-button'>Show Password</button> <button type='submit' class='input-helper' id='connect-button'>Connect</button> </form> </div><div id='finish-div' style='display: none'> <h2>Open your Alexa app</h2> <ol> <li>Go to Skills.</li><li>Enable the FirstBuild Venthood Smarthome skill.</li><li>Enter your Amazon login info!</li></ol> <p>that's it, tell Alexa to turn on the lights!</p><p>(if it's not working re-start the commissioning process</p></div><script src='rsa-utils/jsbn_1.js'></script> <script src='rsa-utils/jsbn_2.js'></script> <script src='rsa-utils/prng4.js'></script> <script src='rsa-utils/rng.js'></script> <script src='rsa-utils/rsa.js'></script> <script src='softap.js'></script> <script src='devnam.js'></script> </body></html>";
const char style_css[] = "@font-face{font-family:'FSEmeric-Medium';src:url(FSEmeric-Medium.woff) format('woff')}@font-face{font-family:'FSEmeric-Regular';src:url(FSEmeric-Regular.woff) format('woff')}html *{height:100%;margin:auto;font-family:'FSEmeric-Regular';background-color:white}span.circle{position:relative;top:-1px;left:-21px;width:15px;height:15px;-webkit-border-radius:7.5px;-moz-border-radius:7.5px;border-radius:7.5px;background:#EF4438;margin-left:40px;display:inline-block}span.circle_open{position:relative;top:-1px;left:-19px;width:12px;height:12px;-webkit-border-radius:9px;-moz-border-radius:9px;border-radius:9px;border:2px solid #EF4438;display:inline-block;margin-left:40px;background-color:white}hr.style{border-top:2px solid #EF4438;position:relative;top:9px;width:180px;margin:auto}body{box-sizing:border-box;min-height:100%;padding:20px;background-color:white;font-weight:400;color:black;margin-top:0;margin-left:auto;margin-right:auto;margin-bottom:0;max-width:400px;text-align:center;border-radius:10px}div{margin-top:25px;margin-bottom:25px;margin-left:auto;margin-right:auto;text-align:center}h2{margin-bottom:25px}button{border-color:black;background-color:white;color:black;border-radius:10px;font-size:15px;font-weight:700;width:170px}button.input-helper{background-color:#F1582C;border-color:#F1582C;color:white}button:disabled{background-color:#bebebe;border-color:#bebebe;color:white}input[type='text'],input[type='password']{background-color:#E9E9E9;color:black;border-color:black;border-radius:10px;height:25px;text-align:center;font-size:15px;margin:5px}::-webkit-input-placeholder{color:#7E7E7E}input:disabled{background-color:#bebebe;border-color:#bebebe}input[type='radio']{position:relative;bottom:-3px;margin:0;border:0;height:1.5em;width:15%}label{position:relative;bottom:2px;padding-top:7px;padding-bottom:7px;padding-left:5%;width:80%;text-align:left;background-color:transparent}input[type='radio']:checked+label{font-weight:700;color:#1c75be}ol{text-align:left}p{text-align:left;margin-left:25px;margin-top:20px;margin-bottom:20px}.radio-div{text-align:left;box-sizing:border-box;margin-bottom:10px;margin-top:10px;background-color:white;color:#6e6e70;border:2px solid #6e6e70;border-radius:10px;width:75%;padding:5px}.scanningerror{font-weight:700;text-align:center}.relative{position:relative;top:-4px;float:left}#email-req{color:red;margin-top:20px;margin-bottom:-3px;margin-left:auto;margin-right:auto;text-align:center;position:relative;left:-72px;font-size:13px;background-color:transparent}#scan-button{min-width:200px;width:75%}#connect-button,#update-button{display:block;min-width:100px;margin-top:20px;margin-left:auto;margin-right:auto;margin-bottom:20px}#password{margin-bottom:10px}#logo{max-width:100%;height:auto}";
const char devnam_js[] = "var scanButton=document.getElementById('scan-button');var venthoodForm=document.getElementById('venthood-form');var updateButton=document.getElementById('update-button');function togObjDisp(childID,dispSet){var elm=document.getElementById(childID);elm.style.display=dispSet}var device_configure=function(a){a.preventDefault();var light=document.getElementById('light').value;var fan=document.getElementById('fan').value;var email=document.getElementById('email').value;if(!email){return!1}var d={idx:0,lightDeviceName:light,fanDeviceName:fan,amznEmail:email};updateButton.innerHTML='Sending configuration...';disableUpdateBtn();console.log('Sending credentials: '+JSON.stringify(d));postRequest(base_url+'cfg',d,device_configure_callback)};var device_configure_callback={success:function(a){console.log('Configuration received.');updateButton.innerHTML='Configuration received...';togObjDisp('logo','none');togObjDisp('hood-device-div','none');togObjDisp('scan-div','block');document.getElementById('stage2').className='circle'},error:function(a,b){console.log('Configuration error: '+a);updateButton.innerHTML='Retry';enableUpdateBtn()}};var disableUpdateBtn=function(){updateButton.disabled=!0};var enableUpdateBtn=function(){updateButton.disabled=!1};if(updateButton.addEventListener){venthoodForm.addEventListener('submit',device_configure)}else if(updateButton.attachEvent){venthoodForm.attachEvent('onsubmit',device_configure)}window.alert('You dont have to change the names but it can be easier if you get more connected lights or fans later on. Good examples of names to use are clear and easy for Alexa, like exhaust, lamp, turbo')";
const char softap_js[] = "var base_url='http://192.168.0.1/';var network_list;var public_key;var rsa=new RSAKey();var scanButton=document.getElementById('scan-button');var connectButton=document.getElementById('connect-button');var showButton=document.getElementById('show-button');var deviceID;var connectForm=document.getElementById('connect-form');var networksDiv=document.getElementById('networks-div');function togObjDisp(childID,dispSet){var elm=document.getElementById(childID);elm.style.display=dispSet}var public_key_callback={success:function(a){console.log('Public key: '+a.b);public_key=a.b;rsa.setPublic(public_key.substring(58,58+256),public_key.substring(318,318+6))},error:function(a,b){console.log(a)}};var device_id_callback={success:function(a){var b=a.id;deviceID=b},error:function(a,b){console.log(a);var c='COMMUNICATION_ERROR';deviceID=c}};var scan=function(){disableButtons();scanButton.innerHTML='Scanning...';connectButton.innerHTML='Connect';togObjDisp('connect-div','none');togObjDisp('networks-div','none');getRequest(base_url+'scan-ap',scan_callback)};var scan_callback={success:function(a){network_list=a.scans;document.getElementById('stage3').className='circle';networksDiv.innerHTML='';if(network_list.length>0)for(var c=0;c<network_list.length;c++){ssid=network_list[c].ssid;console.log(network_list[c]);add_wifi_option(networksDiv,ssid);togObjDisp('connect-div','none')}else networksDiv.innerHTML='<p> No networks found.</p>'},error:function(a){console.log('Scanning error:'+a);networksDiv.innerHTML='<p> Scanning error.</p>'},regardless:function(){scanButton.innerHTML='Re-Scan';enableButtons();togObjDisp('networks-div','block');togObjDisp('connect-div','block')}};var configure=function(a){a.preventDefault();var b=get_selected_network();var c=document.getElementById('password').value;if(!b){window.alert('Please select a network!');return!1}else if(!c){window.alert('Please provide a password!');return!1}console.log(c);var d={idx:0,ssid:b.ssid,pwd:rsa.encrypt(c),sec:b.sec,ch:b.ch};connectButton.innerHTML='Sending credentials...';disableButtons();console.log('Sending credentials: '+JSON.stringify(d));postRequest(base_url+'configure-ap',d,configure_callback)};var configure_callback={success:function(a){console.log('Credentials received.');connectButton.innerHTML='Credentials received...';postRequest(base_url+'connect-ap',{idx:0},connect_callback)},error:function(a,b){console.log('Configure error: '+a);connectButton.innerHTML='Retry';enableButtons();postRequest(base_url+'connect-ap',{idx:0},connect_callback)}};var connect_callback={success:function(a){console.log('Attempting to connect to the cloud.');connectButton.innerHTML='Attempting to connect...';togObjDisp('connect-div','none');togObjDisp('networks-div','none');togObjDisp('scan-div','none');document.getElementById('stage4').className='circle';togObjDisp('finish-div','block')},error:function(a,b){console.log('Connect error:'+a);connectButton.innerHTML='Retry';enableButtons()}};var disableButtons=function(){connectButton.disabled=!0;scanButton.disabled=!0};var enableButtons=function(){connectButton.disabled=!1;scanButton.disabled=!1};var add_wifi_option=function(a,b){var c=document.createElement('INPUT');c.type='radio';c.value=b;c.id=b;c.name='ssid';c.className='radio';var d=document.createElement('DIV');d.className='radio-div';d.appendChild(c);var e=document.createElement('label');e.htmlFor=b;e.innerHTML=b;d.appendChild(e);a.appendChild(d)};var get_selected_network=function(){for(var a=0;a<network_list.length;a++){ssid=network_list[a].ssid;if(document.getElementById(ssid).checked)return network_list[a]}};var toggleShow=function(){var a=document.getElementById('password');inputType=a.type;if(inputType==='password'){showButton.innerHTML='Hide';a.type='text'}else{showButton.innerHTML='Show';a.type='password'}};var getRequest=function(a,b){var c=new XMLHttpRequest();c.open('GET',a,!0);c.timeout=8000;c.send();c.onreadystatechange=function(){if(c.readyState==4)if(b){if(c.status==200){if(b.success)b.success(JSON.parse(c.responseText))}else if(b.error)b.error(c.status,c.responseText);if(b.regardless)b.regardless()}}};function connectionPoll(){var z=new XMLHttpRequest();z.ontimeout=function(){if(document.getElementById('stage4').className==='circle'){return}else if(z.status!==200){window.alert('Your connection has been interrupted.\\n\\nPlease restart the setup process.');window.location.href='http://www.example.com'}};z.open('GET',base_url+'alive',!0);z.timeout=4000;z.onload=function(){if(z.readyState==4&&z.status===200){setTimeout(connectionPoll,5000)}};z.send()}var postRequest=function(a,b,c){var d=JSON.stringify(b);var e=new XMLHttpRequest();e.open('POST',a,!0);e.timeout=4000;e.setRequestHeader('Content-Type','multipart/form-data');e.send(d);e.onreadystatechange=function(){if(e.readyState==4)if(c){if(e.status==200){if(c.success)c.success(JSON.parse(e.responseText))}else if(c.error)c.error(e.status,e.responseText);if(c.regardless)c.regardless()}}};if(scanButton.addEventListener){showButton.addEventListener('click',toggleShow);scanButton.addEventListener('click',scan);connectForm.addEventListener('submit',configure)}else if(scanButton.attachEvent){showButton.attachEvent('onclick',toggleShow);scanButton.attachEvent('onclick',scan);connectForm.attachEvent('onsubmit',configure)}getRequest(base_url+'device-id',device_id_callback);getRequest(base_url+'public-key',public_key_callback);setTimeout(connectionPoll,5000)";
