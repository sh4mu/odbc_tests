using namespace std;

class Statement;
class Environment
{
    public:

    enum State {
        IDLE,
        ENV,
        CONNECTION,
        CLOSED
    };

    public:
        Environment() : 
            mConnectionString("DSN=ttdatasvr64;TTC_SERVER=0-0-2;TCP_PORT=14502"),
            connOutLen(0), m_henv(SQL_NULL_HENV), m_hdbc(SQL_NULL_HDBC), mState(IDLE)
        {
        }

        ~Environment();

        void init();
        void initEnv();
        void initConnect();
        void disable();
        void disableEnv();
        void disableConnect();

        void addStatement(string sql);
        void executeStatement();
    
    public:
        /* General return code for the API */
        SQLHENV m_henv;
        /* Environment handle */
        SQLHDBC m_hdbc;
        /* Connection string */
        string mConnectionString;
        /* Buffer for completed connection string */
        SQLCHAR connOut[1024];
        /* Number of bytes returned in ConnOut */
        SQLSMALLINT connOutLen;
    
        Statement* mStatement;
        State mState;
};