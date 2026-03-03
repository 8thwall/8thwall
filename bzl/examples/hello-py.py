from datetime import *
from dateutil.relativedelta import *

def main():
    now = datetime.now()
    tomorrow = now + relativedelta(days=+1)
    print("Hello World!, tomorrow's date is %s" % tomorrow.date())

if __name__ == "__main__":
    main()
