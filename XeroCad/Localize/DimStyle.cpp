#include "StdAfx.h"
#include "IXeroCad.h"
#include "XhLdsLm.h"

static void GetArcSimuPolyVertex(f3dArcLine* pArcLine, f3dPoint vertex_arr[], int slices)
{
	int i;
	double slice_angle = pArcLine->SectorAngle()/slices;
	if(pArcLine->WorkNorm()==pArcLine->ColumnNorm())	//Բ��
	{
		double sina = sin(slice_angle);	//��Ƭ������
		double cosa = cos(slice_angle);	//��Ƭ������
		vertex_arr[0].Set(pArcLine->Radius());
		for(i=1;i<=slices;i++)
		{
			vertex_arr[i].x=vertex_arr[i-1].x*cosa-vertex_arr[i-1].y*sina;
			vertex_arr[i].y=vertex_arr[i-1].y*cosa+vertex_arr[i-1].x*sina;
		}
		GEPOINT origin=pArcLine->Center();
		f3dPoint axis_x=pArcLine->Start()-origin;
		normalize(axis_x);
		f3dPoint axis_y=pArcLine->WorkNorm()^axis_x;
		for(i=0;i<=slices;i++)
		{
			vertex_arr[i].Set(	origin.x+vertex_arr[i].x*axis_x.x+vertex_arr[i].y*axis_y.x,
				origin.y+vertex_arr[i].x*axis_x.y+vertex_arr[i].y*axis_y.y,
				origin.z+vertex_arr[i].x*axis_x.z+vertex_arr[i].y*axis_y.z);
		}
	}
	else	//��Բ��
	{
		for(i=0;i<=slices;i++)
			vertex_arr[i] = pArcLine->PositionInAngle(i*slice_angle);
	}
}
static void GetCircleSimuPolyVertex(double radius, f3dPoint vertex_arr[], int slices)
{
	double slice_angle = 2*Pi/slices;
	double sina = sin(slice_angle);	//��Ƭ������
	double cosa = cos(slice_angle);	//��Ƭ������
	vertex_arr[0].Set(radius);
	for(int i=1;i<slices;i++)
	{
		vertex_arr[i].x=vertex_arr[i-1].x*cosa-vertex_arr[i-1].y*sina;
		vertex_arr[i].y=vertex_arr[i-1].y*cosa+vertex_arr[i-1].x*sina;
		vertex_arr[i].z=0;
	}
}
void ParseDimBlk(const XERO_DIMSTYLEVAR &dimStyle,const GELINE &line,GEPOINT upper,
			     GEPOINT normal,CXhSimpleList<GELINE> *lpLineList,bool bOnlyStart=false)
{
	GEPOINT lineVec=line.end-line.start;
	normalize(lineVec);
	if( XERO_DIMSTYLEVAR::DIMBLK_ARROWLINE30==dimStyle.m_ciDimBlk||
		XERO_DIMSTYLEVAR::DIMBLK_ARROWLINE90==dimStyle.m_ciDimBlk||
		XERO_DIMSTYLEVAR::DIMBLK_ARROWHOLLOW==dimStyle.m_ciDimBlk)
	{
		double fAngle=15*RADTODEG_COEF;
		if(XERO_DIMSTYLEVAR::DIMBLK_ARROWLINE90==dimStyle.m_ciDimBlk)
			fAngle=45*RADTODEG_COEF;
		double fEdgeLen=dimStyle.m_fArrowSize/cos(fAngle);
		GEPOINT datumPtArr[2]={line.start,line.end};
		f3dPoint datumVecArr[2]={lineVec,-1*lineVec};
		for(int i=0;i<2;i++)
		{
			if(bOnlyStart&&i==1)
				continue;
			GEPOINT arrowEndPtArr[2];
			for(int j=0;j<2;j++)
			{
				f3dPoint arrowVec=datumVecArr[i];
				if(j==0)
					RotateVectorAroundVector(arrowVec,fAngle,normal);
				else 
					RotateVectorAroundVector(arrowVec,-1*fAngle,normal);
				arrowEndPtArr[j]=datumPtArr[i]+arrowVec*fEdgeLen;
				lpLineList->AttachNode(GELINE(datumPtArr[i],arrowEndPtArr[j]));
			}
			if(XERO_DIMSTYLEVAR::DIMBLK_ARROWHOLLOW==dimStyle.m_ciDimBlk)
				lpLineList->AttachNode(GELINE(arrowEndPtArr[0],arrowEndPtArr[1]));
		}
	}
	//else if(XERO_DIMSTYLEVAR::DIMBLK_ARROWSOLID==dimStyle.m_ciDimBlk)
	//else if(XERO_DIMSTYLEVAR::DIMBLK_POINTSOLID==dimStyle.m_ciDimBlk)
	else if(XERO_DIMSTYLEVAR::DIMBLK_POINTHOLLOW==dimStyle.m_ciDimBlk)
	{
		int nSlices= CalArcResolution(dimStyle.m_fArrowSize*0.5,2*Pi,1.0,5.0,36);
		nSlices=min(nSlices,200);
		f3dPoint vertex_arr[201];
		GetCircleSimuPolyVertex(dimStyle.m_fArrowSize*0.5,vertex_arr,nSlices);
		vertex_arr[nSlices]=vertex_arr[0];
		GEPOINT datumPtArr[2]={line.start,line.end};
		for(int i=0;i<2;i++)
		{
			UCS_STRU cs;
			cs.origin=datumPtArr[i];
			cs.axis_x=lineVec;
			cs.axis_z=normal;
			cs.axis_y=cs.axis_z^cs.axis_x;
			normalize(cs.axis_y);
			for(int j=0;j<nSlices;j++)
			{
				GELINE line(vertex_arr[j],vertex_arr[j+1]);
				if(i==1&&bOnlyStart)
					continue;
				coord_trans(line.start,cs,TRUE);
				coord_trans(line.end,cs,TRUE);
				lpLineList->AttachNode(line);
			}
		}
	}
	//else if(XERO_DIMSTYLEVAR::DIMBLK_SQUARESOLID==dimStyle.m_ciDimBlk)
	else if(XERO_DIMSTYLEVAR::DIMBLK_SQUAREHOLLOW==dimStyle.m_ciDimBlk)
	{
		double fEdgeLen=dimStyle.m_fArrowSize;
		GEPOINT datumPtArr[2]={line.start,line.end};
		GEPOINT lineNorm=lineVec;
		RotateVectorAroundVector(lineNorm,0.5*Pi,normal);
		for(int i=0;i<2;i++)
		{
			if(bOnlyStart&&i==1)
				continue;
			GEPOINT rightTopPt=datumPtArr[i]+lineVec*fEdgeLen*0.5;
			rightTopPt+=lineNorm*fEdgeLen*0.5;
			GEPOINT leftBottomPt=datumPtArr[i]-lineVec*fEdgeLen*0.5;
			leftBottomPt-=lineNorm*fEdgeLen*0.5;
			lpLineList->AttachNode(GELINE(rightTopPt,rightTopPt-lineNorm*fEdgeLen));
			lpLineList->AttachNode(GELINE(rightTopPt,rightTopPt-lineVec*fEdgeLen));
			lpLineList->AttachNode(GELINE(leftBottomPt,leftBottomPt+lineNorm*fEdgeLen));
			lpLineList->AttachNode(GELINE(leftBottomPt,leftBottomPt+lineVec*fEdgeLen));
		}
	}
	else if(XERO_DIMSTYLEVAR::DIMBLK_SLASH==dimStyle.m_ciDimBlk)
	{
		GEPOINT slashVec=lineVec;
		double fHalfSlashLen=dimStyle.m_fArrowSize*0.5;
		RotateVectorAroundVector(slashVec,45*RADTODEG_COEF,normal);
		lpLineList->AttachNode(GELINE(line.start-slashVec*fHalfSlashLen,line.start+slashVec*fHalfSlashLen));
		if(!bOnlyStart)
			lpLineList->AttachNode(GELINE(line.end-slashVec*fHalfSlashLen,line.end+slashVec*fHalfSlashLen));
	}
}
void ParseDimension(IDbDimension* pDimension,CXhSimpleList<GELINE> *lpLineList)
{
	if(pDimension==NULL||lpLineList==NULL)
		return;
	char *sDimText=pDimension->GetDimText();	//��ע����
	GEPOINT normal,upper,dimVec;
	GEPOINT dimStart,dimEnd,dimPos;
	XERO_DIMSTYLEVAR dimStyle;
	long dimStyleId=pDimension->GetDimStyleId();
	bool bRetCode=pDimension->BelongDatabase()->GetDimStyle(dimStyleId,&dimStyle);
	ITextStyle* pStyle=pDimension->BelongDatabase()->GetTextStyle(dimStyle.m_idTextStyle);
	if( pDimension->GetDbEntType()==IDbDimension::DbRotatedDimension||
		pDimension->GetDbEntType()==IDbEntity::DbAlignedDimension)
	{
		f3dPoint start_perp,end_perp;
		//1.��ȡ��ע������Ϣ
		if(pDimension->GetDbEntType()==IDbDimension::DbRotatedDimension)
		{
			IDbRotatedDimension *pRotatedDim=(IDbRotatedDimension*)pDimension;
			pRotatedDim->GetLine1Point(dimStart);
			pRotatedDim->GetLine2Point(dimEnd);
			pRotatedDim->GetDimLinePoint(dimPos);
			double rotateAngle=pRotatedDim->GetRotationRadian();
			//1.1 ����ߴ��߷��򡢱�ע����
			upper=pRotatedDim->UpperDirection();
			GEPOINT dimVec1=dimPos-dimStart,dimVec2=dimPos-dimEnd;
			normalize(dimVec1);
			normalize(dimVec2);
			double dd=fabs(dimVec1*dimVec2);
			if(fabs(dimVec1*dimVec2)>EPS_COS)	//dimStart,dimEnd,dimPos���㹲�ߣ�����ָ����UpperDirection��dimVec1ȷ����ע����
				normal=dimVec1^upper;
			else 
				normal=dimVec1^dimVec2;
			normalize(normal);
			dimVec.Set(1,0,0);
			RotateVectorAroundVector(dimVec,rotateAngle,normal);
			upper=dimVec^normal;
			normalize(dimVec);
			normalize(upper);
			//1.2 ����start_perp,end_perp
			SnapPerp(&start_perp,dimPos+dimVec*1000,dimPos-dimVec*1000,dimStart);
			SnapPerp(&end_perp,dimPos+dimVec*1000,dimPos-dimVec*1000,dimEnd);
		}
		else //if(pDimension->GetDbEntType()==IDbEntity::DbAlignedDimension)
		{
			IDbAlignedDimension *pAlignDim=(IDbAlignedDimension*)pDimension;
			pAlignDim->GetLine1Point(dimStart);
			pAlignDim->GetLine2Point(dimEnd);
			pAlignDim->GetDimLinePoint(dimPos);
			dimVec=dimEnd-dimStart;
			normalize(dimVec);
			//1.1 ����ߴ��߷��򡢱�ע����
			SnapPerp(&start_perp,dimPos+dimVec*1000,dimPos-dimVec*1000,dimStart);
			SnapPerp(&end_perp,dimPos+dimVec*1000,dimPos-dimVec*1000,dimEnd);
			//1.2 ��ȡ��ע���򲢼����ע�淨�߷���
			upper=GEPOINT(start_perp)-dimStart;
			normalize(upper);
			GEPOINT input_upper=pAlignDim->UpperDirection();
			if(fabs(upper*dimVec)>EPS_COS)
				upper=input_upper;
			normal=dimVec^upper;
			upper=normal^dimVec;
			normalize(upper);
			normalize(normal);
		}
		//2.���lpLineList
		//2.1 ��ӳߴ��߼��ߴ������˵���ע�������
		GELINE dimLine(start_perp,end_perp);
		lpLineList->AttachNode(dimLine);
		lpLineList->AttachNode(GELINE(start_perp,dimStart));
		lpLineList->AttachNode(GELINE(end_perp,dimEnd));
		//2.2 ��ӳߴ����Ϸ��ӳ���
		if(dimStyle.m_fDimExtend>0)
		{
			GEPOINT extendVec=upper;
			if(extendVec*(GEPOINT(start_perp)-dimStart)<0)
				extendVec*=-1;
			lpLineList->AttachNode(GELINE(start_perp,start_perp+extendVec*dimStyle.m_fDimExtend));
			if(extendVec*(GEPOINT(end_perp)-dimEnd)<0)
				extendVec*=-1;
			lpLineList->AttachNode(GELINE(end_perp,end_perp+extendVec*dimStyle.m_fDimExtend));
		}
		//2.3 ����dimStyle��ʽ��ȡ��ע������GELINE����
		ParseDimBlk(dimStyle,dimLine,upper,normal,lpLineList);
		//2.4 ��ȡ��ע����
		if(strlen(sDimText)<=0)
			sprintf(sDimText,"%.f",DISTANCE(start_perp,end_perp));
		//2.5 ��עλ���ڱ�ע�����
		double fTextWidth=0,fTextHeight=0;
		if(pStyle)
			pStyle->CalTextDrawSize(sDimText,pStyle->Height(),&fTextWidth,&fTextHeight);
		f3dLine dim_line(dimLine.start,dimLine.end);
		if(dim_line.PtInLine(dimPos)==-1)
			lpLineList->AttachNode(GELINE(end_perp,dimPos+dimVec*fTextWidth));
		else if(dim_line.PtInLine(dimPos)==-2)
			lpLineList->AttachNode(GELINE(start_perp,dimPos-dimVec*fTextWidth));
	}
	else if(pDimension->GetDbEntType()==IDbEntity::DbAngularDimension)
	{
		IDbAngularDimension *pAngleDim=(IDbAngularDimension*)pDimension;
		GEPOINT arcPt;
		f3dLine lineArr[2];
		pAngleDim->GetLine1Start(lineArr[0].startPt);
		pAngleDim->GetLine1End(lineArr[0].endPt);
		pAngleDim->GetLine2Start(lineArr[1].startPt);
		pAngleDim->GetLine2End(lineArr[1].endPt);
		pAngleDim->GetArcPoint(arcPt);
		//1.��ֱ���󽻵�
		f3dPoint intersPt;
		Int3dll(lineArr[0],lineArr[1],intersPt);
		//2.������ֱ����arcPt����Բ(����ֱ�߽���ΪԲ��)����Ч����
		//2.1 ����ֱ����Բ����������ϵ
		UCS_STRU dimCS;
		dimCS.origin=intersPt;
		dimCS.axis_x=lineArr[0].endPt-lineArr[0].startPt;
		dimCS.axis_y=lineArr[1].endPt-lineArr[1].startPt;
		dimCS.axis_z=dimCS.axis_x^dimCS.axis_y;
		dimCS.axis_y=dimCS.axis_z^dimCS.axis_x;
		normalize(dimCS.axis_x);
		normalize(dimCS.axis_y);
		normalize(dimCS.axis_z);
		//2.2 ��ȡֱ����Բ����
		coord_trans(arcPt,dimCS,FALSE);
		arcPt.z=0;
		GEPOINT verfiyVec=arcPt;
		f2dCircle cir(arcPt.mod(),0,0);
		GEPOINT arcIntersPtArr[2];
		for(int i=0;i<2;i++)
		{
			GEPOINT intersArr[2];
			GEPOINT startPt=lineArr[i].startPt,endPt=lineArr[i].endPt;
			coord_trans(startPt,dimCS,FALSE);
			coord_trans(endPt,dimCS,FALSE);
			Int2dplc(f2dLine(f2dPoint(startPt.x,startPt.y),f2dPoint(endPt.x,endPt.y)),cir,intersArr[0].x,intersArr[0].y,intersArr[1].x,intersArr[1].y);
			GEPOINT intesVec=intersArr[1]-intersArr[0];
			if(intesVec*verfiyVec>0)
				arcIntersPtArr[i]=intersArr[1];
			else 
				arcIntersPtArr[i]=intersArr[0];
			coord_trans(arcIntersPtArr[i],dimCS,TRUE);
		}
		coord_trans(arcPt,dimCS,TRUE);
		//3.������ע�������GELINE����
		//3.1 ���Ʊ�ע�߼�������
		for(int i=0;i<2;i++)
		{
			int nRetCode=lineArr[i].PtInLine(arcIntersPtArr[i]);
			//-2:��ֱ����,��λ���߶������� 
			//-1:��ֱ����,��λ���߶��յ����
			if(nRetCode!=-1&&nRetCode!=-2)
				continue;
			GELINE line;
			if(nRetCode==-1)
				line.start=lineArr[i].endPt;
			else 
				line.start=lineArr[i].startPt;
			line.end=arcIntersPtArr[i];
			GEPOINT lineVec=line.end-line.start;
			normalize(lineVec);
			lpLineList->AttachNode(line);
			if(dimStyle.m_fDimExtend>0)
				lpLineList->AttachNode(GELINE(line.end,line.end+lineVec*dimStyle.m_fDimExtend));
		}
		//3.2 �����ע����ʱ�������ֵ
		f3dArcLine arcLine;
		f3dPoint line1Vec=arcIntersPtArr[0]-GEPOINT(intersPt),line2Vec=arcIntersPtArr[1]-GEPOINT(intersPt);
		normal=line1Vec^line2Vec;
		normalize(line1Vec);
		normalize(line2Vec);
		normalize(normal);
		upper=arcPt-GEPOINT(intersPt);
		dimVec=upper;
		RotateVectorAroundVector(dimVec,0.5*Pi,normal);
		normalize(upper);
		normalize(dimVec);
		dimStart=arcIntersPtArr[0];
		dimEnd=arcIntersPtArr[1];
		dimPos=arcPt;
		//3.3 ��ӱ�ע�߼���ע������ı�ע��ͷ
		double fDimAngle=cal_angle_of_2vec(line1Vec,line2Vec);
		arcLine.CreateMethod2(arcIntersPtArr[0],arcIntersPtArr[1],normal,fDimAngle);
		int nSlices= CalArcResolution(arcLine.Radius(),fDimAngle,1.0,5.0,36);
		nSlices=min(nSlices,200);
		f3dPoint vertex_arr[201];
		GetArcSimuPolyVertex(&arcLine,vertex_arr,nSlices);
		for(int i=0;i<nSlices;i++)
			lpLineList->AttachNode(GELINE(GEPOINT(vertex_arr[i]),GEPOINT(vertex_arr[i+1])));
		//
		f3dPoint line1Norm=line1Vec,line2Norm=line2Vec;
		RotateVectorAroundVector(line1Norm,0.5*Pi,normal);
		RotateVectorAroundVector(line2Norm,0.5*Pi,normal);
		verfiyVec=arcPt-arcIntersPtArr[0];
		normalize(verfiyVec);
		if(line1Norm*verfiyVec<0)
			line1Norm*=-1;
		if(line2Norm*verfiyVec>0)
			line2Norm*=-1;
		ParseDimBlk(dimStyle,GELINE(arcIntersPtArr[0],arcIntersPtArr[0]+line1Norm*100),upper,normal,lpLineList,true);
		ParseDimBlk(dimStyle,GELINE(arcIntersPtArr[1],arcIntersPtArr[1]+line2Norm*100),upper,normal,lpLineList,true);
		//3.3 ��ȡ��ע����
		if(strlen(sDimText)<=0)
			sprintf(sDimText,"%.f",fDimAngle*DEGTORAD_COEF);
	}
	else if(pDimension->GetDbEntType()==IDbEntity::DbDiametricDimension||
			pDimension->GetDbEntType()==IDbEntity::DbRadialDimension)
	{
		double fLeaderLen=0;
		if(pDimension->GetDbEntType()==IDbEntity::DbDiametricDimension)
		{
			IDbDiametricDimension *pDiaDim=(IDbDiametricDimension*)pDimension;
			//��ȡֱ����ע������Ϣ
			pDiaDim->GetChordPoint(dimStart);
			pDiaDim->GetFarChordPoint(dimEnd);
			fLeaderLen=pDiaDim->GetLeaderLength();
			upper=pDiaDim->UpperDirection();
			//��ȡ��ע���ݼ����ע����
			if(strlen(sDimText)<=0)
				sprintf(sDimText,"��%.f",DISTANCE(dimStart,dimEnd));
		}
		else //if(pDimension->GetDbEntType()==IDbEntity::DbRadialDimension)
		{
			IDbRadialDimension *pRadialDim=(IDbRadialDimension*)pDimension;
			//��ȡֱ����ע������Ϣ
			pRadialDim->GetChordPoint(dimStart);
			pRadialDim->GetCenterPoint(dimEnd);
			fLeaderLen=pRadialDim->GetLeaderLength();
			upper=pRadialDim->UpperDirection();
			if(strlen(sDimText)<=0)
				sprintf(sDimText,"R%.f",DISTANCE(dimStart,dimEnd));
		}
		double fTextWidth=0,fTextHeight=0;
		if(pStyle)
			pStyle->CalTextDrawSize(sDimText,pStyle->Height(),&fTextWidth,&fTextHeight);
		//�����ע�߷������ֳ��򼰱�ע���߷���
		dimVec=dimEnd-dimStart;
		normalize(dimVec);
		normal=dimVec^upper;
		upper=normal^dimVec;
		normalize(normal);
		normalize(upper);
		dimPos=dimStart-dimVec*(fLeaderLen+dimStyle.m_fArrowSize*2);
		//��ӱ�ע����GELINE����
		GELINE startLine(dimStart,dimStart-dimVec*(fLeaderLen+dimStyle.m_fArrowSize*2+fTextWidth));
		GELINE endLine(dimEnd,dimEnd+dimVec*(fLeaderLen+dimStyle.m_fArrowSize*2));
		lpLineList->AttachNode(GELINE(dimStart,dimEnd));
		lpLineList->AttachNode(startLine);
		ParseDimBlk(dimStyle,startLine,upper,normal,lpLineList,true);
		if(pDimension->GetDbEntType()==IDbEntity::DbDiametricDimension)
		{
			lpLineList->AttachNode(endLine);
			ParseDimBlk(dimStyle,endLine,upper,normal,lpLineList,true);
		}
	}
	//������ע���ݲ���ӵ�GELINE����
	if(pStyle)
	{
		CXhSimpleList<GELINE> textLineList;
		pStyle->ParseTextShape(&textLineList,sDimText,pStyle->Height(),0.7,IDbText::AlignBottomCenter);
		UCS_STRU dimTextCS;
		dimTextCS.origin=dimPos+upper*dimStyle.m_fDimGap;
		dimTextCS.axis_x=dimVec;
		dimTextCS.axis_y=upper;
		dimTextCS.axis_z=normal;
		GEPOINT lineStart,lineEnd;
		LIST_NODE<GELINE>* pLineNode=textLineList.EnumFirst();
		while(pLineNode)
		{
			coord_trans(pLineNode->data.start,dimTextCS,TRUE,TRUE);
			coord_trans(pLineNode->data.end,dimTextCS,TRUE,TRUE);
			lpLineList->AttachNode(pLineNode->data);
			pLineNode=textLineList.EnumNext();
		}
	}
}