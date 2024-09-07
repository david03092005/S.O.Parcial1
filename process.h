

using namespace std;

class process {
	private:
		string ID;
		int At;
		int Bt;
		
	public:
		process() = default;
		process(string id, int at, int bt);
		
		string getID();
		
		int getAT();
		
		int getBT();
		void setBT(int newBT);
};