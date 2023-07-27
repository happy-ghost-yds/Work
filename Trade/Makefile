CPP=g++ -w
# CPP_FLAG=-fPIC -O3 -std=c++11 #-DSPDLOG_FMT_EXTERNAL=ON #-ggdb3
CPP_FLAG=-fPIC -std=c++11 # -ggdb3

EXAMPLE_TRAGET=example

SRC = $(wildcard src/*.cpp)
EXCLUDES = src/dcel1Listener.cpp src/DFITCMarketDataListener.cpp \
 src/EFHL2MarketDataListener.cpp src/nProMarketDataListener.cpp

EXAMPLE_SCOURE = $(filter-out $(EXCLUDES),$(SRC))

HEADER = -Iinclude
ESS_HEADER = -Iinclude/trade/rem/include
ESS_LIB = -Linclude/trade/rem/lib

CTP_LIB = -Linclude/ctp
CTP_LINK = -lthostmduserapi_se -lthosttraderapi_se

# uncomment next row to define DCEL1 and link library of DCEL1
# DCEL1 = 1
ifdef DCEL1
	DCEL1_LIB = -Linclude/dcel1
	EXAMPLE_SCOURE += src/dcel1Listener.cpp 
else
	DCEL1_LIB = $(empty) #
endif

# uncomment next row to define NPRO and link library of NPRO
# NPRO = 1
ifdef NPRO
	NPRO_HEADER = -Iinclude/md/nPro/include
	NPRO_LIB = -Linclude/md/nPro/lib
	NPRO_LINK = -lnproplus
	EXAMPLE_SCOURE += src/nProMarketDataListener.cpp
else
	NPRO_HEADER = -Iinclude/md/nPro/include
	NPRO_LIB = $(empty) #
	NPRO_LINK = $(empty) #
endif

# uncomment next row to define EFHL2 and link library of EFHL2
# EFHL2 = 1
ifdef EFHL2
	EFHL2_LIB =	-Linclude/efhl2
	EFHL2_LINK = -lefh_dce_api
	EXAMPLE_SCOURE += src/EFHL2MarketDataListener.cpp
else
	EFHL2_LIB =	$(empty) #
	EFHL2_LINK = $(empty) #
endif

# run sh build.sh inside libuv to build libuv_a.a
# then uncomment next row to define DFITC and compile and link library of DFITC
# DFITC = 1
ifdef DFITC
	LIBUV_HEADER = -Iinclude/libuv/include
	LIBUV_LIB = -Linclude/libuv/lib
	LIBUV_LINK = -luv_a -lssl -lcrypto
	MDP_HEADER = -Iinclude/md/mdp_api/include
	EXAMPLE_SCOURE += src/DFITCMarketDataListener.cpp include/md/mdp_api/src/*.cpp
else
	LIBUV_HEADER = $(empty) #
	LIBUV_LIB = $(empty) #
	LIBUV_LINK = $(empty) #
	MDP_HEADER = -Iinclude/md/mdp_api/include
endif

EXAMPLE_RELY = $(patsubst %.cpp, %.o, $(EXAMPLE_SCOURE))

$(EXAMPLE_RELY):$(EXAMPLE_SCOURE)
	rm -rf $(EXAMPLE_RELY)
	rm -rf $(EXAMPLE_TRAGET)
	$(CPP) $(CPP_FLAG) $(HEADER) $(ESS_HEADER) $(NPRO_HEADER) $(LIBUV_HEADER) $(MDP_HEADER) \
	 $(ESS_LIB) $(DCEL1_LIB) $(CTP_LIB)  $(NPRO_LIB) $(EFHL2_LIB) $(LIBUV_LIB) \
	 $(EXAMPLE_SCOURE) $(CTP_LINK) $(NPRO_LINK) $(EFHL2_LINK) $(LIBUV_LINK) -lEESTraderApi \
	 -ldcel1 -ldl -lrt  -pthread  -o $(EXAMPLE_TRAGET)
	rm -rf $(EXAMPLE_RELY)

clean:
	rm -rf *.o
	rm -rf $(EXAMPLE_TRAGET)
