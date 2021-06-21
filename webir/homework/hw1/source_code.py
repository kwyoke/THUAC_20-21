'''
There will be warning errors scraping the target pages because I disabled certificate verification
'''

import requests
from bs4 import BeautifulSoup
import random
import re
import json

#functions to get top10 results of each SE
def get_top10(html, se):
    top10 = []
    soup = BeautifulSoup(open(html), "html.parser")
    
    if se=='BING':
        rel_results = soup.find_all('li', class_='b_algo')
        i=0
        for info in rel_results:
            rank = i+1
            title = info.find('a').text
            link = info.find('a')['href']
            res_dict = {'rank': rank, 
                        'title': title, 
                        'url': link}
            top10.append(res_dict)
            i+=1
            
            if i==10:
                break
                
    elif se=='GOOGLE':
        rel_results = soup.find_all('div', class_="yuRUbf")
        i=0
        for info in rel_results:
            rank = i+1
            title = info.find('h3').text
            link = info.find('a')['href']
            res_dict = {'rank': rank, 
                        'title': title, 
                        'url': link}
            top10.append(res_dict)
            i+=1
            
            if i==10:
                break
    else:
        print("Error, SE is neither BING nor GOOGLE")
        
            
    return top10

#save as json
def write_serp_info(serpdict, outfilename):
    with open(outfilename, 'w') as outfile:
        json.dump(serpdict, outfile)

#crawl target pages
def save_page(url, outfilename):
    A = ("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2228.0 Safari/537.36",
           "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2227.1 Safari/537.36",
           "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2227.0 Safari/537.36",
           ) #so that websites treat the script as a browser and does not block us

    Agent = A[random.randrange(len(A))]
    headers = {'user-agent': Agent}
    r = requests.get(url, headers=headers, verify=False) #generally not recommended to set verify as false, but no choice, otherwise some webpages cannot access
    if 'html' in r.headers.get('content-type'):
        open(outfilename + '.html', 'wb').write(r.content)
        return
    if 'pdf' in r.headers.get('content-type'):
        open(outfilename + '.pdf', 'wb').write(r.content)
        return
    print('URL is neither html nor pdf')

def crawl_target_results(serpdict, se='BING',querynum='1'):
    for res in serpdict:
        outfilename = 'tp/TP_' + se + '_' + querynum + '_' + str(res['rank']) + '_2020280598'
        save_page(res['url'], outfilename)
		
def main():
	bing_q1_top10 = get_top10('bing_q1.html', "BING")
	google_q1_top10 = get_top10('google_q1.html',"GOOGLE")
	bing_q2_top10 = get_top10('bing_q2.html', "BING")
	google_q2_top10 = get_top10('google_q2.html', "GOOGLE")
	
	outfilenames = ['json/SE_BING_1_2020280598.json', 'json/SE_BING_2_2020280598.json', 'json/SE_GOOGLE_1_2020280598.json','json/SE_GOOGLE_2_2020280598.json']
	write_serp_info(bing_q1_top10, outfilenames[0])
	write_serp_info(bing_q2_top10, outfilenames[1])
	write_serp_info(google_q1_top10, outfilenames[2])
	write_serp_info(google_q2_top10, outfilenames[3])
	print('SERP info json saved')
	
	#crawl target pages
	crawl_target_results(bing_q1_top10, se='BING',querynum='1')
	crawl_target_results(bing_q2_top10, se='BING',querynum='2')
	crawl_target_results(google_q1_top10, se='GOOGLE',querynum='1')
	crawl_target_results(google_q2_top10, se='GOOGLE',querynum='2')
	print('target pages saved')


if __name__ == "__main__":
    main()