using namespace std;

class Environment;
class Statement
{
    public:
        enum State {
            IDLE,
            READY,    
            READY_NOK
        };

    public:
        Statement(string sql) : 
            hstmt(SQL_NULL_HSTMT), mEnvironment(0), mSql(sql), mState(IDLE)
        {}

    void init(Environment* env);
    void executeStatement();

    /* Connection handle */
    SQLHSTMT hstmt;
    /* Statement handle */
    Environment* mEnvironment;
    
    string mSql;
    State mState;
};