class ThreadPoolWrapper {
	public:
	
	int maxThreads;
	int intData[THREAD_DATA_COUNT];
	
	ThreadWrapper* threadPool;
	Singleton* singleton;
	
	bool singleThreaded;
	
	std::vector<int> availIds;
	
	ThreadPoolWrapper() {
		
	}
	
	void init(Singleton* _singleton, int _maxThreads, bool _singleThreaded) {
		int i;
		
		singleton = _singleton;
		
		singleThreaded = _singleThreaded;
		maxThreads = _maxThreads;
		threadPool = new ThreadWrapper[maxThreads];
		
		
		
		for (i = 0; i < maxThreads; i++) {
			threadPool[i].init();
			availIds.push_back(i);
		}
		
		for (i = 0; i < THREAD_DATA_COUNT; i++) {
			intData[i] = 0;
		}
	}
	
	void funcTP(int threadId) {
		
		GamePageHolder* curHolder;
		
		threadPool[threadId].setRunningLocked(true);
		
		curHolder = singleton->gw->getHolderAtId(
			threadPool[threadId].threadDataInt[1],
			threadPool[threadId].threadDataInt[2]
		);
		
		switch(threadPool[threadId].threadDataInt[0]) {
			case E_TT_GENPATHS:
				curHolder->refreshPaths();
			break;
			case E_TT_GENLIST:
				curHolder->generateList();
			break;
		}
		
		//curHolder->genCellData();
		
		threadPool[threadId].setRunningLocked(false);
	}
	void startTP(int threadId) {
		if (threadPool[threadId].threadRunning) {
			
		}
		else {
			threadPool[threadId].threadRunning = true;
			
			if (singleThreaded) {
				funcTP(threadId);
			}
			else {
				threadPool[threadId].threadMain = std::thread(&ThreadPoolWrapper::funcTP, this, threadId);
			}
			
		}
		
	}
	bool stopTP(int threadId) {
		
		GamePageHolder* curHolder;
		
		bool didStop = false;
		if (threadPool[threadId].threadRunning) {
			
			if (singleThreaded) {
				
			}
			else {
				threadPool[threadId].threadMain.join();
			}
			
			curHolder = singleton->gw->getHolderAtId(
				threadPool[threadId].threadDataInt[1],
				threadPool[threadId].threadDataInt[2]
			);
			
			switch(threadPool[threadId].threadDataInt[0]) {
				case E_TT_GENPATHS:
					
				break;
				case E_TT_GENLIST:
					curHolder->fillVBO();
					
				break;
			}
			
			curHolder->lockWrite = false;
			threadPool[threadId].threadRunning = false;
			didStop = true;
			
			availIds.push_back(threadId);
			
		}
		return didStop;
	}
	
	// must set intData first
	bool startThread() {
		int i;
		int q;
		
		
		if (availIds.size() == 0) {
			anyRunning();
		}
		
		if (availIds.size() == 0) {
			return false;
		}
		
		int curId = availIds.back();
		availIds.pop_back();
		
		for (i = 0; i < THREAD_DATA_COUNT; i++) {
			threadPool[curId].threadDataInt[i] = intData[i];
		}
		
		GamePageHolder* curHolder = singleton->gw->getHolderAtId(
			threadPool[curId].threadDataInt[1],
			threadPool[curId].threadDataInt[2]
		);
		
		curHolder->lockWrite = true;
		
		startTP(curId);
		
		return true;
	}
	
	bool anyRunning() {
		
		int q;
		
		bool bAnyRunning = false;
		for (q = 0; q < maxThreads; q++) {
			if (threadPool[q].threadRunning) {
				if (threadPool[q].isReady()) {
					stopTP(q);
				}
				else {
					bAnyRunning = true;
				}
			}
		}
		
		return bAnyRunning;
	}
	
	void stopAll() {
		int q;
		for (q = 0; q < maxThreads; q++) {
			stopTP(q);
		}
	}
	
	~ThreadPoolWrapper() {
		stopAll();
		delete[] threadPool;
	}
};