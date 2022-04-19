import java.io.*;
import java.util.*;
import java.text.*;
import java.util.LinkedList;
import java.util.Queue;



class LSValuePair
{
	Number value1;
	Number value2;
	public LSValuePair(Number value1, Number value2)
	{
		this.value1 = value1;
		this.value2 = value2;
	}
	public Number getValue1()
	{
		return value1;
	}
	public Number getValue2()
	{
		return value2;
	}
	public int hashCode()
	{
		return value1.intValue() + value2.intValue();
	}
	public boolean equals(Object o)
	{
		if (o instanceof LSValuePair)
		{
			LSValuePair values = (LSValuePair)o;
			return value1.equals(values.value1) && value2.equals(values.value2);
		}
		else
		{
			return false;
		}
	}

	public String toString()
	{
		return "(" + value1 + ", " + value2 + ")";
	}
}

class ValueList
{
	private Hashtable valueList;
    private Queue<LSValuePair> valueQueue;
    //private Queue valueQueue;

	public ValueList()
	{
		valueList = new Hashtable();
		valueQueue = new LinkedList<LSValuePair>();
		//valueQueue = new LinkedList();
	}
	public ValueList(String str)
	{
		StringTokenizer tokenizer = new StringTokenizer(str, " ");	
		
		String sourceInstr = tokenizer.nextToken();
		String targetInstr = tokenizer.nextToken();
		int count = new Integer(tokenizer.nextToken()).intValue();			
		valueList = new Hashtable(count);
		valueQueue = new LinkedList<LSValuePair>();

			if (count > 20)
			    count = 20;

		while(count > 0)
		{
			String val1 = tokenizer.nextToken();	
			String val2 = tokenizer.nextToken();
			//	String freq = tokenizer.nextToken();
			valueList.put(new LSValuePair(new Double(val1), new Double(val2)), new Integer(1));

			LSValuePair tmp = new LSValuePair(new Double(val1), new Double(val2));

			valueQueue.add(tmp); //offer
			//	count = count - new Integer(freq);
			count--;
		}
	}
	public Hashtable getHash()
	{
		return valueList;
	}
	public boolean isXandYConstantValued()
	{
		return (1 == valueList.size());
	}
	public boolean isXConstantValued()
	{
		boolean bFirst = true;
		double _x = 0.0;
		double x;
		Enumeration vps = valueList.keys();
		while (vps.hasMoreElements())
		{
			LSValuePair vp = (LSValuePair)vps.nextElement();
			x = vp.getValue1().doubleValue();
			if (bFirst)
			{
				_x = x;
				bFirst = false;
			}
			else
			{
				if (x != _x) return false;
			}
		}
		return true;
	}
	public boolean isYConstantValued()
	{
		boolean bFirst = true;
		double _y = 0.0;
		double y;
		Enumeration vps = valueList.keys();
		while (vps.hasMoreElements())
		{
			LSValuePair vp = (LSValuePair)vps.nextElement();
			y = vp.getValue2().doubleValue();
			if (bFirst)
			{
				_y = y;
				bFirst = false;
			}
			else
			{
				if (y != _y) return false;
			}
		}
		return true;
	}


    public Queue getQueue()
    {
	return valueQueue;
    }
	public ArrayList getList()
	{
		ArrayList list = new ArrayList();		
				
		int minFreq = 10000000;
		Enumeration vps = valueList.keys();
		while (vps.hasMoreElements())
		{
			LSValuePair vp = (LSValuePair)vps.nextElement();
			int freq = ((Integer)valueList.get(vp)).intValue();
			
			if (freq < minFreq) minFreq = freq;			
		}
		
		vps = valueList.keys();
		while (vps.hasMoreElements())
		{
			LSValuePair vp = (LSValuePair)vps.nextElement();
			int freq = ((Integer)valueList.get(vp)).intValue();
			
			freq = freq/minFreq;
			freq = (freq > 100) ? 100: freq;
			
			for (int i = 0; i < freq; i++)
			{
				list.add(vp);
			}			
		}
		return list;
	}
	public int getFreqSum()
	{
		int freqSum = 0;
		Enumeration vps = valueList.keys();
		while (vps.hasMoreElements())
		{
			LSValuePair vp = (LSValuePair)vps.nextElement();
			freqSum += ((Integer)valueList.get(vp)).intValue();
		}
		return freqSum;
	}
	public String toString()
	{
		String str = "";
		Enumeration vps = valueList.keys();
		while (vps.hasMoreElements())
		{
			LSValuePair vp = (LSValuePair)vps.nextElement();
			str += vp.toString() + " ";
		}
		return str;
	}
	public int hashCode()
	{
		int h = 0;		
		Enumeration vps = valueList.keys();
		while (vps.hasMoreElements())
		{
			LSValuePair vp = (LSValuePair)vps.nextElement();
			h += vp.hashCode();
		}
		return h;
	}

	public boolean equals(Object obj)
	{
		if (obj instanceof ValueList)
		{
			Hashtable list = ((ValueList)obj).valueList;
			if (list.size() == valueList.size())
			{
				new Throwable().printStackTrace();
				return true; // myEquals(list);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	public void merge(ValueList valueList1)
	{
		Hashtable hash1 = valueList1.valueList;
		Enumeration vps1 = hash1.keys();
		while (vps1.hasMoreElements())
		{
			LSValuePair vp1 = (LSValuePair)vps1.nextElement();
			Integer freq1 = (Integer)hash1.get(vp1);

			Integer freq2 = (Integer)valueList.get(vp1);
			if (null == freq2)
			{
				valueList.put(vp1, freq1);
			}
			else
			{
				valueList.put(vp1, new Integer(freq2.intValue()+ freq1.intValue()));
			}			
		}
	}
	
}


class FlowItem
{
	public ValueList valueList = new ValueList();
	public void addValues(ValueList values)
	{
		this.valueList.merge(values);
	}	
}

class FlowID
{
	public long targetInstr;
	public long sourceInstr;
				
	public FlowID(long targetInstr, long sourceInstr)
	{
		this.targetInstr = targetInstr;
		this.sourceInstr = sourceInstr;		
	}

	public long FlowIDHashCode() { return 10*targetInstr + 100*sourceInstr; }

	public boolean equals(Object o) { 
		FlowID fid = (FlowID)o;
		return (fid.targetInstr == targetInstr) && (fid.sourceInstr == sourceInstr);
	}

	public String toString() { return sourceInstr  + " " + targetInstr ; }

	public long toStringSrc() { return sourceInstr ; }

	public long toStringTarg() { return targetInstr ; }
}


public class ReadFileDistinct
{
	static double ERROR = -1000.0;		
	
public static void main(String args[])
{
	Hashtable hashFlows = new Hashtable();

	
	try{

	    FileWriter logFW =  new FileWriter("strengthDistinct.log"); // runtime logs 

	    FileWriter logQueue =  new FileWriter("queueDistinct.log"); // runtime logs 

	    FileWriter logConst =  new FileWriter("constantDistinct.log"); // runtime logs 
	    FileWriter logNotConst =  new FileWriter("notConstantDistinct.log"); // runtime logs 

	    FileWriter logV =  new FileWriter("ArraysValuesDistinct.log"); // runtime logs 

	    //YOU NEED TO TYPE BELOW THE PATH TO valuesSlicing.log AND UNCOMMNENT THE BELOW LINE.
	    //BufferedReader bufr = new BufferedReader(new FileReader("/.............../valuesSlicing.log"));


	    
		
		while(bufr.ready())
		{
				String line1 = bufr.readLine();
				
				
				StringTokenizer tokenizer = new StringTokenizer(line1, " ");   	

				long sourceInstr = new Long(tokenizer.nextToken()).longValue();

				long targetInstr = new Long(tokenizer.nextToken()).longValue();
			

				if (targetInstr == sourceInstr){
				    System.out.println(line1);
				    continue;
				}

				int counter = new Integer(tokenizer.nextToken()).intValue();

			       if (counter >=5){


				FlowID fid = new FlowID(targetInstr, sourceInstr);		

				ValueList vl = new ValueList(line1);
				
				hashFlows.put(fid, vl);	
				}				
	
				
		}
		
		
		Enumeration fids = hashFlows.keys();

		// compute strengths
		int maxSampleSize = -1;
		int histoSampleSize[] = new int[20000]; 
		int histoEntropy[] = new int[20000]; 
		int histoR[] = new int[20000]; 
		int histoETA[] = new int[20000]; 
		double INC = 0.01;
		double maxEntropy = -1;
		double maxR = -1;
		double maxETA = -1;
		double sumEntropy = 0;
		double sumR = 0;
		double sumETA = 0;
		int numFlows = 0;
		int nErrors = 0;	
	
		int nUsedFlows = 0;
		int nTotalFlows = 0;
		int nIgnoredFlows = 0;
		int [][] valueTypes1 = new int[3][3]; //0=scalar, 1=string, 2=non-string
		int [][] valueTypes2 = new int[2][2]; //0=scalar, 1=string,non-string		
		int [][] valueConstants = new int[2][2];
		int numZeroEntropy = 0;
		int numZeroETA = 0;
		int numZeroR = 0;
		int numZeroAll = 0;
		
		while (fids.hasMoreElements())
		{
			FlowID fid = (FlowID)fids.nextElement();
			if (null != hashFlows.get(fid)){
			
			    ValueList  valueList = (ValueList) hashFlows.get(fid);
			    ArrayList valuesList = ((ValueList) hashFlows.get(fid)).getList();

			    Queue valueQueue = ((ValueList) hashFlows.get(fid)).getQueue();

			    boolean bXConstant = false;
			    boolean bYConstant = false;

			    if (valueList.isXConstantValued()){
				bXConstant = true;
				logConst.write(fid.toStringSrc()+"\n");

			    }
			    else{
				logNotConst.write(fid.toStringSrc()+"\n");
			    }

			    if (valueList.isYConstantValued()){
				bYConstant = true;
				logConst.write(fid.toStringTarg()+"\n");
			    }
			    else{
				logNotConst.write(fid.toStringTarg()+"\n");
			    }

			    if (bXConstant && bYConstant){
				valueConstants[0][0]++;
			    }
			    else if (bXConstant && !bYConstant){
				valueConstants[0][1]++;
			    }
			    else if (!bXConstant && bYConstant){
				valueConstants[1][0]++;
			    }
			    else if (!bXConstant && !bYConstant)
				valueConstants[1][1]++;

			    double [] x = new double[valuesList.size()];
			    double [] y = new double[valuesList.size()];
				
				for (int i = 0; i < valuesList.size(); i++)
				{
					LSValuePair item = (LSValuePair)valuesList.get(i);
					Number value1 = item.getValue1();
					Number value2 = item.getValue2();
					x[i] = value1.doubleValue();
					y[i] = value2.doubleValue();
				}	
				
				
				double [] u = new double[valueQueue.size()];
				double [] v = new double[valueQueue.size()];

				int tmp_queue_size =  valueQueue.size();

				for (int i = 0; i <tmp_queue_size ; i++)
				{
					LSValuePair item = (LSValuePair)valueQueue.poll();
					Number value1 = item.getValue1();
					Number value2 = item.getValue2();
					u[i] = value1.doubleValue();
					v[i] = value2.doubleValue();
				}	
				

			       	double r = (bXConstant || bYConstant) ? 0.0 : computeLinear(x, y);
				double eta = (bXConstant || bYConstant) ? 0.0 : computeETA(x, y);
				double entropy = computeEntropy(x.length, x, y);
				
				logFW.write(fid.toString() + " " + r + " " + eta + " " + entropy + "\n\n");
				
			
				double mr = (bXConstant || bYConstant) ? 0.0 : computeLinear(u, v);
				double meta = (bXConstant || bYConstant) ? 0.0 : computeETA(u, v);
				double mentropy = computeEntropy(u.length, u, v);


				
				logQueue.write(fid.toString() + " " + mr + " " + meta + " " + mentropy + "\n\n");
				//end u and v

			}					
				
		}

		logFW.flush();
		logFW.close();
		logV.flush();
		logV.close();
		logQueue.flush();
		logQueue.close();
		logConst.flush();
		logConst.close();
		logNotConst.flush();
		logNotConst.close();
	}
	catch(IOException e){
		System.err.println(e.toString());
	}
	

}



static double computeLinear(double x[], double y[])
	{
		int n = x.length;
		double Sxy,Sxx,Sx,Sy,Syy;
		Sxy = (double)0.0;
		Sxx = (double)0.0;
		Syy = (double)0.0;
		Sx = (double)0.0;
		Sy = (double)0.0;
		for (int i = 0; i < n; i++)
		{
			Sxy += x[i]*y[i];
			Syy += y[i]*y[i];
			Sx += x[i];
			Sy += y[i];
			Sxx += x[i]*x[i];
		}
		
		double r = (double)((n*Sxy-Sx*Sy)/Math.sqrt((n*Sxx-Sx*Sx)*(n*Syy-Sy*Sy)));
		r = Math.abs(r);

		if ((r >= 0.0) && (r <= 1.01))
		{
			return (r > 1.0) ? 1.0 : r;
		}
		else
		{
			return ERROR;
		}
	}

	static double computeETA(double x[], double y[])
	{
		// ETA = variance(Y_bar)/variance(Y)
		// variance = (1/N)*Sum(yi - meanY)^2
		
		double [] y_ = new double[y.length];
		int n = x.length;
		double Sy = 0.0;
		double Sy_ = 0.0;
		double Sx = 0.0;
		double varianceY = 0.0;
		double varianceY_ = 0.0;
		double varianceX = 0.0;
		
		// varianceY
		for (int i = 0; i < n; i++)
		{
			Sy += y[i];
		}
		double meanY = Sy/n;
		for (int i = 0; i < n; i++)
		{
			varianceY += (y[i]-meanY)*(y[i]-meanY);
		}
		varianceY = varianceY/n;
				

		// varianceX
		for (int i = 0; i < n; i++)
		{
			Sx += x[i];
		}
		double meanX = Sx/n;
		for (int i = 0; i < n; i++)
		{
			varianceX += (x[i]-meanX)*(x[i]-meanX);
		}
		varianceX = varianceX/n;

		// Y_ = average for each category of x
		for (int i = 0; i < n; i++)
		{
			double sumY = 0;
			int nCount = 0;
			for (int j = 0; j < n; j++)
			{
				if (x[j] == x[i])
				{
					sumY += y[j];
					nCount++;
				}
			}
			y_[i] = sumY/nCount;
		}

		// varianceY_
		for (int i = 0; i < n; i++)
		{
			Sy_ += y_[i];
		}
		double meanY_ = Sy_/n;	
		for (int i = 0; i < n; i++)
		{
			varianceY_ += (y_[i]-meanY_)*(y_[i]-meanY_);
		}
		varianceY_ = varianceY_/n;

		// ***Final***
		double rETA = varianceY_/varianceY;
		rETA = Math.abs(rETA);
				
		if ((rETA >= 0.0) && (rETA <= 1.01))
		{
			return (rETA > 1.0) ? 1.0 : rETA;
		}
		else
		{
			return ERROR;
		}
	}

	
	static double computeEntropy(int n, double x[], double y[])
	{
		// Flow from x to y if H(xs | yt) < H(xs)
		// -Sj P(yt=yj)[Si P(xs=xi| yt=yj) lg P(xs=xi| yt=yj)] <  -Si P(xs=xi) lg P(xs=xi)]
		
		// compute Hx = -Si P(xs=xi) lg P(xs=xi)]
		Hashtable Px = computeP(n, x);
		double Hx = 0.0;
		for (Enumeration e = Px.elements(); e.hasMoreElements() ;) 
		{
			Integer p = (Integer)e.nextElement();
			Hx += (p.doubleValue()/n) * Math.log(p.doubleValue()/n)/Math.log(2);			
		}
		Hx = -Hx;		
		
		
		// compute H(xs | yt) = -Sj P(yt=yj)[Si P(xs=xi| yt=yj) lg P(xs=xi| yt=yj)]
		Hashtable Py = computeP(n, y);
		double Hxy = 0.0;
		for (Enumeration e = Py.keys(); e.hasMoreElements() ;) 
		{
			Double yvalue = (Double)e.nextElement();
			double py = ((Integer)Py.get(yvalue)).doubleValue()/n;
			
			Hashtable Pxy = computePxy(n, x, y, yvalue.doubleValue());
			double temp = 0.0;
			int n2 = 0;
			for (int i = 0; i < n; i++) {
				if (y[i] != yvalue.doubleValue()) continue;
				n2++;
			}
			for (Enumeration e2 = Pxy.elements(); e2.hasMoreElements() ;) 
			{
				Integer pxy = (Integer)e2.nextElement();
				temp += (pxy.doubleValue()/n2) * Math.log(pxy.doubleValue()/n2)/Math.log(2);
			}
	
			Hxy += py*temp;
		}
		Hxy = -Hxy;		
			
		if (Hx!=0){
			return (Hx-Hxy)/Hx;
		}
		else 
			return Hx-Hxy;
	}
	
	static Hashtable computePxy(int n, double x[], double y[], double yvalue)
	{
		Hashtable table = new Hashtable();
		for (int i = 0; i < n; i++)
		{
			if (y[i] != yvalue) continue;
			
			Integer count = (Integer)table.get(new Double(x[i]));
			if (null == count)
			{
				table.put(new Double(x[i]), new Integer(1));
			}
			else
			{
				table.put(new Double(x[i]), new Integer(count.intValue()+1));
			}
		}
		
		return table;
	}
	
	static Hashtable computeP(int n, double x[])
	{
		Hashtable table = new Hashtable();
		for (int i = 0; i < n; i++)
		{
			Integer count = (Integer)table.get(new Double(x[i]));
			if (null == count)
			{
				table.put(new Double(x[i]), new Integer(1));
			}
			else
			{
				table.put(new Double(x[i]), new Integer(count.intValue()+1));
			}
		}
		
		return table;
	}

}
