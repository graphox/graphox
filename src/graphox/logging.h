#ifndef GRAPHOX_LOGGING_H
#define GRAPHOX_LOGGING_H
	class Logging
	{
		public:
			bool Shouldshow(int);
			void log(int, const char *);
		private:
			int level = 0;
	
	}
#endif
