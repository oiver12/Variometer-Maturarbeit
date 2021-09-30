// ////////////////////////////////////////////////////////////////////////////////////////////
// Class name:	CFiFo
// 					
// Purpose:		FiFo.h:	interface for the CFiFo template class .
//						Implements a First-In-First-Out buffer which can hold
//						any type of class represented by ELEMENT.
//						A very nasty thing is that the ClassView from VisualC++ is not able to
//						handle templates unless their implementation is placed in the header
//						file. Example of usage: We want to have an instance of CFiFo 
//						named m_RxQueue which holds up to 1000 characters :
//						CFiFo<char, 1000> m_RxQueue; does this job. Now we can add data to
//						m_RxQueue.Put('a'); and so on...
//
//				NOTE.	Modifications of the selected properties of the stored elements may 
//						be implemented as a series of GetNext() and PuOver() calls. The queue must 
//						be protected by CTxCriticalSection for the duration of this operation.
//
// Modifications:		Creation BEC GmbH MB
//	
//										
// ////////////////////////////////////////////////////////////////////////////////////////////


#if !defined(FIFO_H)
#define FIFO_H

template <class ELEMENT, int BufferSize> class CFiFo  
{
	
public:
	
	typedef unsigned long POSITION;

	CFiFo(void)
	{
		m_Buffer[0] = ELEMENT();
		this->Reset();
	}
	
	virtual ~CFiFo(void)
	{
	}
	
	
	// ********************************************************************************************
	// Function name:		Put()	
	//									
	// Description:			Inserts a new element only if there is unoccupied place in the buffer.
	//									
	//									
	// Modifications:		Creation BEC GmbH MB
	//				
	// ********************************************************************************************
	
	bool Put(ELEMENT Element) 
	{
		if ((m_InIdx == m_OutIdx) && (m_bInIncremented == true))
		{
			return false; // queue is full
		}
		
		m_bInIncremented = true;
		m_Buffer[m_InIdx] = Element;
		m_InIdx = (m_InIdx+1)%BufferSize;
		
		return true;
	}

	bool isFull()
	{
		return (m_InIdx == m_OutIdx) && (m_bInIncremented == true);
	}
	
	
	
	// ********************************************************************************************
	// Function name:		PutOver()
	//									
	// Description:			Inserts a new element if there is unocupied place in the buffer, or 
	//						replaces the oldes element with the new one
	//									
	// Modifications:		Creation BEC GmbH MB
	//				
	// ********************************************************************************************
	
	bool PutOver(ELEMENT Element) 
	{ 
		while (!Put(Element))
		{
			ELEMENT ElementToDelete;
			Get(ElementToDelete); 
		}
		
		return true;
	}
	
	
	
	// ********************************************************************************************
	// Function name:		Get()
	//									
	// Description:			If the queue is not empty, copies the oldest element from the buffer to 
	//						the input ELEMENT address and moves m_OutIdx (and m_TrendPos in one case) 
	//						to the next oldest element. 
	//						Used in PutOver(). Clients should use GetNext() to retrieve elements. 
	//									
	// Modifications:		Creation BEC GmbH MB
	//		
	// ********************************************************************************************
	
	bool Get(ELEMENT& Element) 
	{
		if ((m_InIdx == m_OutIdx) && (m_bInIncremented == false)) // queue is empty
		{
			return false; 
		}
		m_bInIncremented = false;
		Element = m_Buffer[m_OutIdx];

		unsigned long Idx = m_OutIdx;
		m_OutIdx = (m_OutIdx+1)%BufferSize;
		if(m_TrendPos == Idx) // also move TrendPos
		{
			m_TrendPos = m_OutIdx;
		}
		return true;
	}
	
	
	// ********************************************************************************************
	// Function name:		GetNext(Element)
	//									
	// Description:			If the buffer is not empty, copies the oldest element from the buffer to 
	//						the input ELEMENT address and increments moves m_TrendPos to the next element.
	//									
	// Modifications:		Creation BEC GmbH MB
	//			
	// ********************************************************************************************
	
	bool GetNext(ELEMENT& Element, bool FirstElement = false)
	{
		POSITION Position = 0;

		if (FirstElement)
		{
			Position = m_OutIdx;
			
			if ((m_InIdx == Position) && (m_bInIncremented == false)) // queue is empty
			{
				return false;
			}
		}
		else
		{
			Position = m_TrendPos;
		}

		return this->GetNextAt(Position, Element);

	}
	
	// ********************************************************************************************
	// Function name:	POSITION GetHeadPosition()
	//									
	// Description:		Gets the position of the head element of this fifo
	//									
	// Modifications:	Creation BEC GmbH MB	
	//			
	// ********************************************************************************************
	
	POSITION GetHeadPosition(void)
	{
		return m_OutIdx;
	}
	
	// ********************************************************************************************
	// Function name:	Element GetNext(Position)
	//									
	// Description:		Gets the element identified by Position, then sets Position to the 
	//					POSITION value of the next entry in the fifo. You can use GetNext in a forward
	//					iteration loop if you establish the initial position with a call to GetHeadPosition
	//									
	// Modifications:	Creation BEC GmbH MB	
	//			
	// ********************************************************************************************
	
	bool GetNextAt(POSITION& Position, ELEMENT& Element)
	{
		bool bRetVal = true;
		if(m_InIdx == m_OutIdx)
		{
			if(!m_bInIncremented)
			{// queue is empty
				Position = m_InIdx;
				//pElement = NULL;
				//return false; 
				bRetVal = false;
			}
			// else Queue is full
		}
		else if(m_OutIdx<m_InIdx)
		{
			if( (Position>=m_InIdx) ||
				(Position<m_OutIdx) )
			{// Position is out of range
				Position = m_OutIdx;// Move Position to the oldest element
				//return false; 
				bRetVal = false;
			}
		}
		else
		{
			if( (Position>=m_InIdx) &&
				(Position<m_OutIdx) )
			{// Position is out of range
				Position = m_OutIdx;// Move Position to the oldest element
				//pElement = NULL;
				//return false; 
				bRetVal = false;
			}
		}
		if( (Position == (m_InIdx-1)) ||
			((Position == BufferSize-1) && (m_InIdx == 0)) )
		{// This is the last element and the position can not be incremented
			//pElement = &(m_Buffer[Position]);
			Element = (m_Buffer[Position]);
			//return false; 
			bRetVal = false;
		}

	

		if(bRetVal)
		{
			Element = (m_Buffer[Position]);
			Position = (Position+1)%BufferSize;
		}
		m_TrendPos = Position;

		return bRetVal; 
	}
	
	// ********************************************************************************************
	// Function name:	Reset()
	//									
	// Description:		Clears the contents of the fifo. Buffer is not dealocated.
	//									
	// Modifications:	Creation BEC GmbH MB	
	//			
	// ********************************************************************************************
	
	void Reset() // Clears the contents of the fifo. Buffer is not dealocated.
	{
		m_InIdx		= 0;
		m_OutIdx	= 0;
		m_TrendPos	= 0;
		m_bInIncremented = false;
	}
	
	
	// ********************************************************************************************
	// Function name:	GetFillSize()
	//									
	// Description:		Returns the absolut fill count level of the fifo (not percent)
	//									
	// Modifications:	Creation BEC GmbH MB	
	//			
	// ********************************************************************************************
	unsigned long GetFillSize() // Returns the absolut fill level of the fifo (not percent)
	{
		if(m_InIdx == m_OutIdx)
		{
			if(!m_bInIncremented)
			{
				return 0; // queue is empty
			}
			else
			{
				return BufferSize;
			}
		}
		
		if(m_OutIdx < m_InIdx)
		{
			return (m_InIdx-m_OutIdx);
		}
		else
		{
			return (m_InIdx+BufferSize-m_OutIdx);
		}
	}
	
	
	// ********************************************************************************************
	// Function name:	SetReverseCount()
	//									
	// Description:		Sets the trend or histdump POSITION (pointer) with the count reverse to the EndPosition.
	//					E.g. if count is 0 the POSITION for trend or histdump will be the Endposition.
	//					If count is larger than the FillSize, the POSITION will be the HeadPosition.
	//									
	// Modifications:	Creation BEC GmbH MB	
	//			
	// ********************************************************************************************
	bool SetReverseCount(unsigned long Count)
	{
		if(Count>=GetFillSize()) // Since Count is larger than FillSize return the last element
		{
			m_TrendPos = m_OutIdx;
			return true;
		}
		
		if(m_InIdx>=Count)
		{
			m_TrendPos = (m_InIdx-Count);
			return true;
		}
		m_TrendPos = (BufferSize-Count+m_InIdx);
		return true;
	}
	

	// return element at index
	void peek(unsigned int Idx,ELEMENT& Element)
	{
		Element = m_Buffer[Idx];
	}

	// return writing index
	int top()
	{
		return m_InIdx;
	}



protected:
	unsigned long		m_OutIdx;				// index of the oldest element
	unsigned long		m_InIdx;				// index for inserting a new element
	POSITION	m_TrendPos;				// current positon for GetNext()
	bool		m_bInIncremented;		// false if queue is empty
	ELEMENT		m_Buffer[BufferSize];
};

#endif // !defined(FIFO_H)
