#include "spdlog/spdlog.h"

#include "strategy/exampleBuyStrategy.h"

static const unsigned int defaultOrderSize = 1;

static unsigned int quantize(double x, double unit)
{
    double quotient = std::round(x / unit);
    return static_cast<unsigned int>(quotient);
}

CExampleBuyStrategyProcessor::CExampleBuyStrategyProcessor(EES_ExchangeID exchangeId) : CStrategyProcessor(exchangeId)
{
    Env& env = Env::getInstance();
    m_exampleBuyConfig = {};
    m_exampleBuyConfig.instrument = env["exampleBuyStrategy"]["instrument"];
    m_exampleBuyConfig.targetInventory = env["exampleBuyStrategy"]["targetInventory"];
    m_exampleBuyConfig.minPriceChange = env["exampleBuyStrategy"]["minPriceChange"];
    m_exampleBuyConfig.multiplier = env["exampleBuyStrategy"]["multiplier"];
}

CExampleBuyStrategyProcessor::~CExampleBuyStrategyProcessor()
{
    if (m_callbackThread.joinable()) {
        m_callbackThread.join();
    }
}

unsigned int CExampleBuyStrategyProcessor::getInstrumentLimitPrice(std::string instrument, int side)
{
    double _price = CStrategyProcessor::getInstrumentLimitPrice(instrument, side);
    double quotient = std::round(_price / m_exampleBuyConfig.minPriceChange);
    return static_cast<unsigned int>(quotient);
}

void CExampleBuyStrategyProcessor::startTraderCallbackWorker()
{
    isTraderCallbackStarted = true;
    m_callbackThread = std::thread(&CExampleBuyStrategyProcessor::_traderCallbackWorker, this);
}

void CExampleBuyStrategyProcessor::_traderCallbackWorker()
{
    Base_OrderExecutionField exec;
    while (currentHolding < m_exampleBuyConfig.targetInventory)
    {
        exec = m_TraderCallbackQueue.pop(); //blocking
        this->_processTraderCallback(&exec);
    }
}

void CExampleBuyStrategyProcessor::_processTraderCallback(Base_OrderExecutionField* pExec)
{
    int _token = pExec->m_ClientOrderToken;
    spdlog::info("Token={0} is being processed", _token); 
    if (m_openOrders.count(_token))
    {
        // update pendingVolume
        int _direction = m_openOrders[_token].direction;
        unsigned int _price = m_openOrders[_token].price;
        // m_openOrders[_token].volumeBeforeEst = 0;
        m_openOrders[_token].pendingVolume -= (pExec->m_Quantity);

        if (m_openOrders[_token].pendingVolume <= 0)
        {
            m_toRemovePriceLevels.insert(m_openOrders[_token].price);
            m_openOrders.erase(_token);
        }
        // update holdings for the matched limit order
        if (_direction == 1)
        {
            currentHolding += (pExec->m_Quantity);
        }
        spdlog::info("Token={0} Executed Amt={1} currentHolding={2}", _token, _direction*pExec->m_Quantity, 
            currentHolding);
    }
}

void CExampleBuyStrategyProcessor::processData(std::vector<Mydata>* data, int lastIndex)
{
    if (!isTraderCallbackStarted) this->startTraderCallbackWorker();
    // spdlog::info("enter CExampleBuyStrategyProcessor::processData {0} {1} {2}", currentLongHolding, currentShortHolding, m_exampleBuyConfig.targetInventory);
    if (currentHolding >= m_exampleBuyConfig.targetInventory) return;

    // spdlog::info("impl CExampleBuyStrategyProcessor::processData {}", lastIndex);
    auto lastSnapshot = (*data)[lastIndex];
    unsigned int lastA1 = quantize(getField(lastSnapshot, DATA_FIELD_SELL_PRICE_1), m_exampleBuyConfig.minPriceChange);
    unsigned int lastA1V =  static_cast<unsigned int>(getField(lastSnapshot, DATA_FIELD_SELL_VOLUME_1));
    unsigned int lastB1 = quantize(getField(lastSnapshot, DATA_FIELD_BUY_PRICE_1), m_exampleBuyConfig.minPriceChange);
    unsigned int lastB1V =  static_cast<unsigned int>(getField(lastSnapshot, DATA_FIELD_BUY_VOLUME_1));
    spdlog::info("[Index={0}] B1={1} B1V={2} A1={3} A1V={4}", lastIndex, 
        lastB1*m_exampleBuyConfig.minPriceChange, lastB1V,
        lastA1*m_exampleBuyConfig.minPriceChange, lastA1V
    );

    unsigned int orderSizeB = defaultOrderSize;
    
    // update relative location in queue
    if (localMdIndex.size() > 0)
    {
        auto prevSnapshot = (*data)[localMdIndex[localMdIndex.size()-1]];

        // open new positions
        if (m_activePriceLevels.count(lastB1) == 0)
        {
            int _token = trader->newOrder(m_exampleBuyConfig.instrument, EES_SideType_open_long, 
                    lastB1*m_exampleBuyConfig.minPriceChange, orderSizeB);
            m_openOrders[_token] = {
                _token,
                1,
                orderSizeB,
                orderSizeB,
                lastB1
            };
            m_activePriceLevels.insert(lastB1);
            spdlog::info("[Index={0}] Token={1} LimitBuyOrder of size={2} at price={3}", lastIndex, 
                _token,
                orderSizeB,
                lastB1*m_exampleBuyConfig.minPriceChange
            );
        }
    }

    for (auto _level: m_toRemovePriceLevels)
    {
        m_activePriceLevels.erase(_level);
    }
    m_toRemovePriceLevels.clear();

    // ++
    localMdIndex.push_back(lastIndex);
}
