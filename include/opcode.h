typedef uint8_t u8;
typedef uint16_t u16;



class Processor
{
private:
	union {
		u16 af;
		struct {
			byte a;
			byte f;
		};
	};
	union {
		doublebyte bc;
		struct {
			byte b;
			byte c;
		};
	};
	union {
		doublebyte de;
		struct {
			byte d;
			byte e;
		};
	};
	union {
		doublebyte hl;
		struct {
			byte h;
			byte l;
		};
	};
	doublebyte sp;
	doublebyte pc;

	byte memory[0x10000];
	void execute()
}


	
