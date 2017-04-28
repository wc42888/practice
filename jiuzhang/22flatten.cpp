class Solution {
public:
    // @param nestedList a list of NestedInteger
    // @return a list of integer
    
    vector<int> result;
    
    vector<int> flatten(vector<NestedInteger> &nestedList) {

        deNest(nestedList);
        
        return result;
    }
    
    void deNest(vector<NestedInteger> &nestedList){
        for(int i =0; i<nestedList.size(); ++i){
            if(nestedList[i].isInteger()){
                result.push_back(nestedList[i].getInteger());
            }
            else{
                vector<NestedInteger> nested = nestedList[i].getList();
                deNest(nested);
                //deNest(nestedList[i].getList());
            }
        }        
    }
};
