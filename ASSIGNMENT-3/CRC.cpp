#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

//Checks if a string contains all zeroes or not.
int is_zero(string s)
{
    int n=s.length();
    int i,j,k;

    for(i=0 ; i<n ; i++)
    {
        if(s[i]=='1')
        {
            return 0;
        }
    }

    return 1;
}

//Generates random integers in the given range.
vector <int> rand_gen(int n , int l , int r , int o)
{
    vector <int> rand_num;
    map <int,int> mp;
    int i,j,k;

    while(n>0)
    {
        int num=l+rand()%(r+1-l);

        if((o==0 || (o==1 && num%2==1)) && mp.find(num)==mp.end())
        {
            rand_num.push_back(num);

            mp[num]++;

            n--;
        }
    }

    return rand_num;
}

//Computes binary minus(XOR) for two bit strings.
string bin_minus(string a , string b)
{
    int n=a.length();
    int m=b.length();
    int i,j,k;

    if(n>m)
    {
        for(i=0 ; i<n-m ; i++)
        {
            b='0'+b;
        }
    }
    else
    {
        for(i=0 ; i<m-n ; i++)
        {
            a='0'+a;
        }
    }

    string c="";

    for(i=0 ; i<n ; i++)
    {
        if(a[i]==b[i]) c+='0';
        else c+='1';
    }

    return c;
}

//Computes remainder of the division a bit string by another bit string.
string bin_div(string num , string den)
{
    int n=num.length();
    int m=den.length();
    int i,j,k;

    string divs1=den;
    string divs0="";
    string divd="";
    string rem;

    for(i=0 ; i<m ; i++)
    {
        divs0+='0';

        divd+=num[i];
    }

    j=m;

    while(1)
    {
        if(divd[0]=='1') rem=bin_minus(divd,divs1);
        else rem=bin_minus(divd,divs0);

        rem.erase(rem.begin());

        if(j<n) divd=rem+num[j++];
        else break;
    }

    return rem;
}

int main(int argv,char* argc[])
{
    //Seed to generate different set of random numbers.
    srand(time(NULL));

    fstream fiptr,foptr;

    //File handling.
    fiptr.open(argc[1],ios::in);
    foptr.open(argc[2],ios::out); 

    string message;
    int t=1;

    //Taking each message string as input from file.
    while(getline(fiptr,message))
    {
        string gen_pol="100000111";
        string mess_pol;
        string rem_pol;
        string trans_pol;

        mess_pol=message;

        int n=mess_pol.length();
        int r=gen_pol.length()-1;
        int i,j,k;

        string mess_pol_1=mess_pol;

        for(i=0 ; i<r ; i++)
        {
            mess_pol_1+='0';
        }

        //Finding remainder.
        rem_pol=bin_div(mess_pol_1,gen_pol);

        trans_pol=bin_minus(mess_pol_1,rem_pol);

        string corr_trans_pol;

        vector <int> rand_errs=rand_gen(10,3,n+r,1);

        foptr << "String-" << t++ << endl;
        foptr << endl;

        foptr << "Original String: " << mess_pol << endl;
        foptr << "Original String with CRC: " << trans_pol << endl;
        foptr << endl;

        //Generating 10 random corrupted strings.
        for(i=0 ; i<10 ; i++)
        {
            string err_pol;
            string rem;

            corr_trans_pol=trans_pol;

            vector <int> rand_err_bits=rand_gen(rand_errs[i],0,n+r-1,0);

            for(j=0 ; j<rand_errs[i] ; j++)
            {
                corr_trans_pol[rand_err_bits[j]]=97-corr_trans_pol[rand_err_bits[j]];
            }

            err_pol=corr_trans_pol;

            rem=bin_div(err_pol,gen_pol);

            int e1=is_zero(err_pol);
            int e2=is_zero(rem);

            foptr << "Corrupted String: " << corr_trans_pol << endl;
            foptr << "Number of Errors Introduced: " << rand_errs[i] << endl;
            foptr << "CRC Check: ";

            if(e2==0) foptr << "Failed(Error is Detected)" << endl;
            else if(e1!=0) foptr << "No Error." << endl;
            else if(e1==0 && e2!=0) foptr << "Passed(Error is not Detected)" << endl;

            foptr << endl;
        }
        foptr << endl;

        vector <int> rand_bursty_errs=rand_gen(5,100,110,0);

        //Generating 5 random corrupted strings with bursty errors.
        for(i=0 ; i<5 ; i++)
        {
            string err_pol;
            string rem;

            corr_trans_pol=trans_pol;

            for(j=0 ; j<6 ; j++)
            {
                corr_trans_pol[rand_bursty_errs[i]+j]=97-corr_trans_pol[rand_bursty_errs[i]+j];
            }

            err_pol=bin_minus(trans_pol,corr_trans_pol);

            rem=bin_div(err_pol,gen_pol);

            int e1=is_zero(err_pol);
            int e2=is_zero(rem);

            foptr << "Corrupted String: " << corr_trans_pol << endl;
            foptr << "Number of Errors Introduced: " << 6 << endl;
            foptr << "CRC Check: ";

            if(e2==0) foptr << "Failed(Error is Detected)" << endl;
            else if(e1!=0) foptr << "No Error." << endl;
            else if(e1==0 && e2!=0) foptr << "Passed(Error is not Detected)" << endl;

            foptr << endl;
        }
        foptr << endl;
    }
    foptr << endl;

    fiptr.close();
    foptr.close();

    return 0;
}