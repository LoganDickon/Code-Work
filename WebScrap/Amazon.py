import csv
from bs4 import BeautifulSoup
from msedge.selenium_tools import Edge, EdgeOptions
from selenium import webdriver

def get_url(search_term):
	template = "https://www.amazon.com/s?k={}&ref=nb_sb_noss_2"
	search_term = search_term.replace(' ', '+')

	# add term query to url
	url = template.format(search_term)

	# add page query placeholder
	url += '&page{}'

	return template.format(search_term)

def extract_record(item):
	#Extract and return all item info

	# Items Description
	atag = item.h2.a
	description = atag.text.strip()

	# Items URL
	url = 'https://www.amazon.com' + atag.get('href')

	try:
		# Items Price
		price_parent = item.find('span', 'a-price')
		price = price_parent.find('span', 'a-offscreen').text
	
	except AttributeError:
		return

	try:
		# Items Ratings
		rating = item.i.text
		review_count = item.find('span', {'class' : 'a-size-base'}).text
	
	except AttributeError:
		rating = ''
		review_count = ''

	result = (description, price, rating, review_count, url)

	return result

def main(search_term):
	# Start Web Driver
	driver = webdriver.Edge('C:/WebScraping-main/msedgedriver.exe')
	options = EdgeOptions()
	options.use_chromium = True

	records = []
	url = get_url(search_term)

	for page in range(1, 21):
		driver.get(url.format(page))
		soup = BeautifulSoup(driver.page_source, 'html.parser')
		results = soup.find_all('div', {'data-component-type': 's-search-result'})

		for item in results:
			record = extract_record(item)
			if record:
				records.append(record)

	driver.close()

	# Save Data To CSV File
	file = ('C:/WebScraping-main/Scraped Files/' + search_term + '.csv').lower()

	with open(file, 'w', newline='', encoding='utf-8') as f:
		writer = csv.writer(f)
		writer.writerow(['Product', 'Price $', 'Rating', 'Num of Reviews', 'URL'])
		writer.writerows(records)

print("Welcome! This program takes any product search on Amazon.com and extracts the data to a Spreadsheet(.csv)")
print("Tip!: What you type here will be the name of the .csv file.")
Search = input('Enter Search: ')
main(Search)
