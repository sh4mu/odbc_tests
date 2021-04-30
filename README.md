# tttests
Using tttest, conclude the following:
* The SQL environment, connection are established, statement is prepared and the statement can be executed
* Reusing the same environment and connection, the statement can be done repeatedly
* Once the connection to DB TT fails, the statement cannot be executed (when disabling IP takes 1 minute timeout, when 0-0-2 returns exception)
* Once the connection to DB TT comes up againg, the Connection and Statement handlers must be re-created. Only the Environment Handler can be reused

# CommonSDK Data Access
Check CommonSDK LLD docs

# How to improve CommonSDK Resilience/Failover
https://celfocusjira.atlassian.net/browse/CFNUCM-3274
